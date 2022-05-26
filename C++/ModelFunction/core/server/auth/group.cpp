#include "group.h"

Group::Group(string id, int type, string name, uint64_t rightBit, uint64_t previewBit, uint64_t playbackBit, uint64_t ctrlBit, int level)
{
    m_id          = id;
    m_type        = type;
    m_name        = name;
    m_rightBit    = rightBit;
    m_previewBit  = previewBit;
    m_playbackBit = playbackBit;
    m_ctrlBit     = ctrlBit;
    m_level       = level;
}

Group::~Group()
{
}

int
Group::type()
{
    return m_type;
}

string
Group::id()
{
    return m_id;
}

uint64_t
Group::right()
{
    return m_rightBit;
}

void
Group::modify(string name, uint64_t rightBit, uint64_t previewBit, uint64_t playbackBit, uint64_t ctrlBit, int level)
{
    m_name        = name;
    m_rightBit    = rightBit;
    m_previewBit  = previewBit;
    m_playbackBit = playbackBit;
    m_ctrlBit     = ctrlBit;
    m_level       = level;
}
