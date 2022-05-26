#include "uuid.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>                                                                                                                                       
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>                                                                                                                                          
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static int have_random_source(void)
{
	struct stat s;

	return (!stat("/dev/random", &s) || !stat("/dev/urandom", &s));
}


int random_get_fd(void)
{
	int i, fd;
	struct timeval	tv;

	gettimeofday(&tv, 0);
	fd = open("/dev/urandom", O_RDONLY);
	if (fd == -1)
		fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
	if (fd >= 0) {
		i = fcntl(fd, F_GETFD);
		if (i >= 0)
			fcntl(fd, F_SETFD, i | FD_CLOEXEC);
	}
	srand((getpid() << 16) ^ getuid() ^ tv.tv_sec ^ tv.tv_usec);

	/* Crank the random number generator a few times */
	gettimeofday(&tv, 0);
	for (i = (tv.tv_sec ^ tv.tv_usec) & 0x1F; i > 0; i--)
		rand();

	return fd;
}

void random_get_bytes(void *buf, size_t nbytes)
{
	size_t i, n = nbytes;
	int fd = random_get_fd();
	int lose_counter = 0;
	unsigned char *cp = (unsigned char *) buf;

	if (fd >= 0) {
		while (n > 0) {
			ssize_t x = read(fd, cp, n);
			if (x <= 0) {
				if (lose_counter++ > 16)
					break;
				continue;
			}
			n -= x;
			cp += x;
			lose_counter = 0;
		}

		close(fd);
	}

	/*
	 * We do this all the time, but this is the only source of
	 * randomness if /dev/random/urandom is out to lunch.
	 */
	for (cp = (unsigned char *)buf, i = 0; i < nbytes; i++)
		*cp++ ^= (rand() >> 7) & 0xFF;

	return;
}

static int get_node_id(unsigned char *node_id)
{
	return 0;
}

static int get_clock(uint32_t *clock_high, uint32_t *clock_low,
		     uint16_t *ret_clock_seq, int *num)
{
	int		adjustment = 0;
	struct timeval	last = {0, 0};
	int		state_fd = -2;
	FILE		*state_f;
	uint16_t		clock_seq;
	struct timeval			tv;
	uint64_t			clock_reg;
	mode_t				save_umask;
	int				len;
	int				ret = 0;

	if (state_fd == -2) {
		save_umask = umask(0);
		state_fd = open(LIBUUID_CLOCK_FILE, O_RDWR|O_CREAT|O_CLOEXEC, 0660);
		(void) umask(save_umask);
		if (state_fd != -1) {
			state_f = fdopen(state_fd, "r+" "");
			if (!state_f) {
				close(state_fd);
				state_fd = -1;
				ret = -1;
			}
		}
		else
			ret = -1;
	}
	if (state_fd >= 0) {
		rewind(state_f);
		while (flock(state_fd, LOCK_EX) < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				continue;
			fclose(state_f);
			close(state_fd);
			state_fd = -1;
			ret = -1;
			break;
		}
	}
	if (state_fd >= 0) {
		unsigned int cl;
		unsigned long tv1, tv2;
		int a;

		if (fscanf(state_f, "clock: %04x tv: %lu %lu adj: %d\n",
			   &cl, &tv1, &tv2, &a) == 4) {
			clock_seq = cl & 0x3FFF;
			last.tv_sec = tv1;
			last.tv_usec = tv2;
			adjustment = a;
		}
	}

	if ((last.tv_sec == 0) && (last.tv_usec == 0)) {
		random_get_bytes(&clock_seq, sizeof(clock_seq));
		clock_seq &= 0x3FFF;
		gettimeofday(&last, 0);
		last.tv_sec--;
	}

try_again:
	gettimeofday(&tv, 0);
	if ((tv.tv_sec < last.tv_sec) ||
	    ((tv.tv_sec == last.tv_sec) &&
	     (tv.tv_usec < last.tv_usec))) {
		clock_seq = (clock_seq+1) & 0x3FFF;
		adjustment = 0;
		last = tv;
	} else if ((tv.tv_sec == last.tv_sec) &&
	    (tv.tv_usec == last.tv_usec)) {
		if (adjustment >= 10)
			goto try_again;
		adjustment++;
	} else {
		adjustment = 0;
		last = tv;
	}

	clock_reg = tv.tv_usec*10 + adjustment;
	clock_reg += ((uint64_t) tv.tv_sec)*10000000;
	clock_reg += (((uint64_t) 0x01B21DD2) << 32) + 0x13814000;

	if (num && (*num > 1)) {
		adjustment += *num - 1;
		last.tv_usec += adjustment / 10;
		adjustment = adjustment % 10;
		last.tv_sec += last.tv_usec / 1000000;
		last.tv_usec = last.tv_usec % 1000000;
	}

	if (state_fd >= 0) {
		rewind(state_f);
		len = fprintf(state_f,
			      "clock: %04x tv: %016lu %08lu adj: %08d\n",
			      clock_seq, last.tv_sec, last.tv_usec, adjustment);
		fflush(state_f);
		if (ftruncate(state_fd, len) < 0) {
			fprintf(state_f, "                   \n");
			fflush(state_f);
		}
		rewind(state_f);
		flock(state_fd, LOCK_UN);
	}

	*clock_high = clock_reg >> 32;
	*clock_low = clock_reg;
	*ret_clock_seq = clock_seq;
	return ret;
}

void uuid_unpack(const uuid_t in, struct uuid *uu)
{
	const uint8_t	*ptr = in;
	uint32_t		tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	tmp = (tmp << 8) | *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_low = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_mid = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->time_hi_and_version = tmp;

	tmp = *ptr++;
	tmp = (tmp << 8) | *ptr++;
	uu->clock_seq = tmp;

	memcpy(uu->node, ptr, 6);
}

void uuid_pack(const struct uuid *uu, uuid_t ptr)
{
	uint32_t	tmp;
	unsigned char	*out = ptr;

	tmp = uu->time_low;
	out[3] = (unsigned char) tmp;
	tmp >>= 8;
	out[2] = (unsigned char) tmp;
	tmp >>= 8;
	out[1] = (unsigned char) tmp;
	tmp >>= 8;
	out[0] = (unsigned char) tmp;

	tmp = uu->time_mid;
	out[5] = (unsigned char) tmp;
	tmp >>= 8;
	out[4] = (unsigned char) tmp;

	tmp = uu->time_hi_and_version;
	out[7] = (unsigned char) tmp;
	tmp >>= 8;
	out[6] = (unsigned char) tmp;

	tmp = uu->clock_seq;
	out[9] = (unsigned char) tmp;
	tmp >>= 8;
	out[8] = (unsigned char) tmp;

	memcpy(out+10, uu->node, 6);
}

void __uuid_generate_random(uuid_t out, int *num)
{
	uuid_t	buf;
	struct uuid uu;
	int i, n;

	if (!num || !*num)
		n = 1;
	else
		n = *num;

	for (i = 0; i < n; i++) {
		random_get_bytes(buf, sizeof(buf));
		uuid_unpack(buf, &uu);

		uu.clock_seq = (uu.clock_seq & 0x3FFF) | 0x8000;
		uu.time_hi_and_version = (uu.time_hi_and_version & 0x0FFF)
			| 0x4000;
		uuid_pack(&uu, out);
		out += sizeof(uuid_t);
	}
}

void uuid_generate_random(uuid_t out)
{
	int	num = 1;
	/* No real reason to use the daemon for random uuid's -- yet */

	__uuid_generate_random(out, &num);
}


int __uuid_generate_time(uuid_t out, int *num)
{
	static unsigned char node_id[6];
	static int has_init = 0;
	struct uuid uu;
	uint32_t	clock_mid;
	int ret;

	if (!has_init) {
		if (get_node_id(node_id) <= 0) {
			random_get_bytes(node_id, 6);
			/*
			 * Set multicast bit, to prevent conflicts
			 * with IEEE 802 addresses obtained from
			 * network cards
			 */
			node_id[0] |= 0x01;
		}
		has_init = 1;
	}
	ret = get_clock(&clock_mid, &uu.time_low, &uu.clock_seq, num);
	uu.clock_seq |= 0x8000;
	uu.time_mid = (uint16_t) clock_mid;
	uu.time_hi_and_version = ((clock_mid >> 16) & 0x0FFF) | 0x1000;
	memcpy(uu.node, node_id, 6);
	uuid_pack(&uu, out);

	return ret;
}

static int uuid_generate_time_generic(uuid_t out) {

	return __uuid_generate_time(out, 0);
}

void uuid_generate_time(uuid_t out)
{
	(void)uuid_generate_time_generic(out);
}

void uuid_generate(uuid_t out)
{
	if (have_random_source())
		uuid_generate_random(out);
	else
		uuid_generate_time(out);
}


static void uuid_unparse_x(const uuid_t uu, char *out, const char *fmt)
{
	struct uuid uuid;

	uuid_unpack(uu, &uuid);
	sprintf(out, fmt,
		uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
		uuid.clock_seq >> 8, uuid.clock_seq & 0xFF,
		uuid.node[0], uuid.node[1], uuid.node[2],
		uuid.node[3], uuid.node[4], uuid.node[5]);
}

void uuid_unparse(const uuid_t uu, char *out)
{
	uuid_unparse_x(uu, out, FMT_DEFAULT);
}
