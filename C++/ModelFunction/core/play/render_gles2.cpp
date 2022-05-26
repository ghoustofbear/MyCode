#include "render_gles2.h"
#include "tls_error.h"
#include "renderlistener.h"
#include <math.h>

const char * codeFragShader = \
"precision mediump float;											\n" \
"uniform sampler2D yTexture; 										\n" \
"uniform sampler2D uTexture; 										\n" \
"uniform sampler2D vTexture; 										\n" \
"varying vec2 vTexCoor;												\n" \
"void main()														\n" \
"{																	\n" \
"	float y = texture2D(yTexture, vTexCoor).r;						\n" \
"	float u = texture2D(uTexture, vTexCoor).r;							    		\n" \
"	float v = texture2D(vTexture, vTexCoor).r;										\n" \
"	vec3 yuv = vec3(y, u, v);												        \n" \
"	vec3 offset = vec3(16.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0);					\n" \
"	mat3 mtr = mat3(1.0, 1.0, 1.0, -0.001, -0.3441, 1.772, 1.402, -0.7141, 0.001);	\n" \
"	vec4 curColor = vec4(mtr * (yuv - offset), 1);									\n" \
"	gl_FragColor = curColor;									    				\n" \
"}																	                \n" \
;



//顶点着色器脚本代码
const char * codeVertexShader = \
"attribute vec3 aPosition;							\n" \
"uniform mat4 uMVPMatrix;	 						\n" \
"attribute vec2 aTexCoor; 							\n" \
"varying vec2 vTexCoor;		 						\n" \
"void main() 										\n" \
"{ 													\n" \
"	gl_Position = uMVPMatrix * vec4(aPosition, 1); 	\n" \
" 	vTexCoor = aTexCoor;							\n" \
"} 													\n" \
;


#define PI 3.14159265f
#define UNIT 1
#define TEXTURE_COOR_UNIT 1

//渲染顶点坐标数据
const float dataVertex[] =
{
    -1 * UNIT, 1 * UNIT, 0,
    -1 * UNIT, -1 * UNIT, 0,
    1 * UNIT, 1 * UNIT, 0,
    1 * UNIT, -1 * UNIT, 0
};
//渲染纹理坐标数据
const float dataTexCoor[] =
{
    0 * TEXTURE_COOR_UNIT, 0 * TEXTURE_COOR_UNIT,
    0 * TEXTURE_COOR_UNIT, 1 * TEXTURE_COOR_UNIT,
    1 * TEXTURE_COOR_UNIT, 0 * TEXTURE_COOR_UNIT,
    1 * TEXTURE_COOR_UNIT, 1 * TEXTURE_COOR_UNIT
};

#define ANDROIDGLES__

GLES2Render::GLES2Render(HWND wnd, bool useTxt)
    : Render(wnd)
{
    m_type     = R_OPENGLES2;
    m_useTxt   = useTxt;
    m_init = false;

    m_width = 0;
    m_height = 0;
    m_buffer = NULL;
}

GLES2Render::~GLES2Render()
{
    if (m_EGLDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(m_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_EGLContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_EGLDisplay, m_EGLContext);
        }
        if (m_EGLSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(m_EGLDisplay, m_EGLSurface);
        }
        eglTerminate(m_EGLDisplay);
    }
    m_EGLDisplay = EGL_NO_DISPLAY;
    m_EGLContext = EGL_NO_CONTEXT;
    m_EGLSurface = EGL_NO_SURFACE;

    


    if (m_buffer)
        free(m_buffer);
    m_buffer = NULL;
}

int
GLES2Render::init()
{
    if (m_useTxt)
    {
        m_init = true;
#ifdef ANDROIDGLES__
        //获取一个EGLDisplay对象
        m_EGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        
        if (m_EGLDisplay != EGL_NO_DISPLAY)
        {
            EGLint majorVersion;
            EGLint minorVersion;
            EGLBoolean bsucc = eglInitialize(m_EGLDisplay, &majorVersion, &minorVersion);
            
            if (bsucc != EGL_TRUE)
            {
                ERRE_PLY("EGL eglInitialize failed \n");
                return -1;
            }
            ERRE_PLY("EGL version major:%d, minor:%d \n", majorVersion, minorVersion);
        }

        const EGLint attribs[] =
        {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
        };
        EGLint	numConfigs;
        eglChooseConfig(m_EGLDisplay, attribs, &m_EGLConfig, 1, &numConfigs);
        EGLint 	format;
        eglGetConfigAttrib(m_EGLDisplay, m_EGLConfig, EGL_NATIVE_VISUAL_ID, &format);


        //使用上面的格式，根据Android窗口，对显示进行拉伸
//        uint32_t window_width = ANativeWindow_getWidth((EGLNativeWindowType)m_wnd);
//        uint32_t window_height = ANativeWindow_getHeight((EGLNativeWindowType)m_wnd);

//        ERRE_PLY("EGL window size width:%d, height:%d \n", window_width, window_height);

//        int ret = ANativeWindow_setBuffersGeometry((EGLNativeWindowType)m_wnd, window_width, window_height, format);
//        if (ret){
//            ERRE_PLY("ANativeWindow_setBuffersGeometry(format) returned error %d", ret);
//            return -1;
//        }
        //用上面构造的display获取一个surface，这个surface可以认为是当前的帧缓存
        m_EGLSurface = eglCreateWindowSurface(m_EGLDisplay, m_EGLConfig, (EGLNativeWindowType)m_wnd, NULL);

        EGLint attr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
        m_EGLContext = eglCreateContext(m_EGLDisplay, m_EGLConfig, 0, attr);
        if (!m_EGLContext)
        {
            ERRE_PLY("EGL eglCreateContext failed \n");
            return -1;
        }
        if (eglMakeCurrent(m_EGLDisplay, m_EGLSurface, m_EGLSurface, m_EGLContext) == EGL_FALSE)
        {
            ERRE_PLY("EGL eglMakeCurrent failed \n");
            return -1;
        }

        if (!eglQuerySurface(m_EGLDisplay, m_EGLSurface, EGL_WIDTH, &m_width) ||
            !eglQuerySurface(m_EGLDisplay, m_EGLSurface, EGL_HEIGHT, &m_height)
            )
        {
            return -1;
        }
        ERRE_PLY("EGL eglQuerySurface EGL_WIDTH:%d EGL_HEIGHT:%d", m_width, m_height);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_DEPTH_TEST);
        ////////////////////
#endif
        gles_init();

        return 0;
    }
    else
    {
        return -1;
    }
}

int 
GLES2Render::gles_init()
{
    memset(&m_glInstance, 0, sizeof(m_glInstance));

    //编译链接shader
    m_glInstance.pProgram = buildProgram(codeVertexShader, codeFragShader);
    ERRE_PLY("m_glInstance.pProgram:%d", m_glInstance.pProgram);
    //获取mvp矩阵的索引
    m_glInstance.maMVPMatrixHandle = glGetUniformLocation(m_glInstance.pProgram, "uMVPMatrix");
    ERRE_PLY("m_glInstance.maMVPMatrixHandle:%d", m_glInstance.maMVPMatrixHandle);
    //获取顶点坐标索引
    m_glInstance.maPositionHandle = glGetAttribLocation(m_glInstance.pProgram, "aPosition");
    ERRE_PLY("m_glInstance.maPositionHandle:%d", m_glInstance.maPositionHandle);
    //获取纹理坐标索引
    m_glInstance.maTexCoorHandle = glGetAttribLocation(m_glInstance.pProgram, "aTexCoor");
    ERRE_PLY("m_glInstance.maTexCoorHandle:%d", m_glInstance.maTexCoorHandle);
    //获取采样器索引
    m_glInstance.myTextureHandle = glGetUniformLocation(m_glInstance.pProgram, "yTexture");
    m_glInstance.muTextureHandle = glGetUniformLocation(m_glInstance.pProgram, "uTexture");
    m_glInstance.mvTextureHandle = glGetUniformLocation(m_glInstance.pProgram, "vTexture");
    ERRE_PLY("m_glInstance.myTextureHandle:%d m_glInstance.muTextureHandle:%d m_glInstance.mvTextureHandle:%d", m_glInstance.myTextureHandle, m_glInstance.muTextureHandle, m_glInstance.mvTextureHandle);
    //获取对象名称 这里分别返回1个用于纹理对象的名称，后面为对应纹理赋值时将以这个名称作为索引
    glGenTextures(1, &m_glInstance.yTexture);
    glGenTextures(1, &m_glInstance.uTexture);
    glGenTextures(1, &m_glInstance.vTexture);
    ERRE_PLY("m_glInstance.yTexture:%d m_glInstance.uTexture:%d m_glInstance.vTexture:%d", m_glInstance.yTexture, m_glInstance.uTexture, m_glInstance.vTexture);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    ERRE_PLY("GLES2 init success \n");
}

int
GLES2Render::changeWndSize(int width, int height)
{
    ERRE_PLY("changeLayout() width = %d, height = %d", width, height);
    m_glInstance.vWidth = width;
    m_glInstance.vHeight = height;

    unsigned int angle;
    unsigned int eW, eH;
    float vRadio, radio;
    angle = 270;
    vRadio = (float)height / width;
    if (angle == 90 || angle == 270)
    {
        radio = ((float)m_glInstance.pWidth / m_glInstance.pHeight);
    }
    else
    {
        radio = ((float)m_glInstance.pHeight / m_glInstance.pWidth);
    }
    if (vRadio < radio)
    {
        eH = m_glInstance.vHeight;
        eW = (unsigned int)(eH / radio);
    }
    else
    {
        eW = m_glInstance.vWidth;
        eH = (unsigned int)(eW * radio);
    }
    ERRE_PLY("changeLayout eW = %d, eH = %d, radio = %f, vRadio = %f, instance->pHeight = %d, instance->pWidth = %d", eW, eH, radio, vRadio, m_glInstance.pHeight, m_glInstance.pWidth);
    glViewport(0, 0, eW, eH);
    return 0;
}

void
GLES2Render::onYUV(void *data, int size, int w, int h)
{
    if (!m_init)
        init();
    
    if (m_glInstance.pWidth != w || m_glInstance.pHeight != h)
    {
        m_glInstance.pWidth = w;
        m_glInstance.pHeight = h;

        if (m_buffer)
            free(m_buffer);

        m_buffer = (char*)malloc(size);

        m_glInstance.yBuffer = m_buffer;
        m_glInstance.uBuffer = m_buffer + w*h;
        m_glInstance.vBuffer = m_buffer + (w*h*5)/4;
    }

    if (m_buffer)
        memcpy(m_buffer, data, size);

    //gl_draw_frame
    drawFrame();
    //end_render_frame

	if (m_listener)
    {
        m_listener->onHDC(NULL, m_wnd, w, h);
    }

    //交换display中显示图像缓存的地址和后台图像缓存的地址，将当前计算出的图像缓存显示
#ifdef ANDROIDGLES__
    EGLBoolean res = eglSwapBuffers(m_EGLDisplay, m_EGLSurface);
    if (res == EGL_FALSE){
        ERRE_PLY("eglSwapBuffers Error %d", eglGetError());
    }
#endif
}

void
GLES2Render::onRGB(void *data, int size, int w, int h)
{

}

GLuint 
GLES2Render::bindTexture(GLenum texture_n, GLuint texture_id, const void *buffer, GLuint w, GLuint h)
{
    //2.绑定纹理
    glActiveTexture(texture_n);//eg:GL_TEXTURE0
    //1.1绑定纹理id
    glBindTexture(GL_TEXTURE_2D, texture_id);
    //2.3设置采样模式
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //1.2输入纹理数据
    glTexImage2D(GL_TEXTURE_2D,
        0,//GLint level
        GL_LUMINANCE,//GLint internalformat
        w,//GLsizei width
        h,// GLsizei height,
        0,//GLint border,
        GL_LUMINANCE,//GLenum format,
        GL_UNSIGNED_BYTE,//GLenum type,
        buffer//const void * pixels
        );
    return texture_id;
}

unsigned int 
GLES2Render::buildShader(const char* source, GLenum shaderType)
{
    GLuint shaderHandle = glCreateShader(shaderType);
    if (shaderHandle)
    {
        glShaderSource(shaderHandle, 1, &source, NULL);
        glCompileShader(shaderHandle);
        GLint compiled = 0;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char* buf = (char*)malloc(infoLen); 
                if (buf)
                {
                    glGetShaderInfoLog(shaderHandle, infoLen, &infoLen, buf);
                    ERRE_PLY("\t glCompileShader fail, resen:%s \n\n", buf);
                    free(buf);
                }
                glDeleteShader(shaderHandle);
                shaderHandle = 0; 
            }
        }
    }
    return shaderHandle; 
}



unsigned int 
GLES2Render::buildProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    unsigned int vertexShader = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    unsigned int programHandle = glCreateProgram();

    if (programHandle)
    {
        glAttachShader(programHandle, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(programHandle, fragmentShader);
        checkGlError("glAttachShader");

        glLinkProgram(programHandle);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*)malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(programHandle, bufLength, NULL, buf);
                    ERRE_PLY("\t glLinkProgram fail, resen:%s \n\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(programHandle);
            programHandle = 0;
        }
    }
    return programHandle;
}

void 
GLES2Render::checkGlError(const char* op)
{
    GLint error;
    for (error = glGetError(); error; error = glGetError())
    {
        //log("error::after %s() glError (0x%x)\n", op, error);
        ERRE_PLY("GLES2 error, after %s() glError (0x%x)\n", op, error);
    }
}


float*
getRotateM(float* rm, int rmOffset, float a, float x, float y, float z)
{
    if (rm == NULL)
    {
        rm = (float*)malloc(sizeof(float) * 16);
        memset(rm, 0, sizeof(float) * 16);
    }
    rm[rmOffset + 3] = 0;
    rm[rmOffset + 7] = 0;
    rm[rmOffset + 11] = 0;
    rm[rmOffset + 12] = 0;
    rm[rmOffset + 13] = 0;
    rm[rmOffset + 14] = 0;
    rm[rmOffset + 15] = 1;
    a *= (float)(PI / 180.0f);
    float s = (float)sin(a);
    float c = (float)cos(a);
    if (1.0f == x && 0.0f == y && 0.0f == z)
    {
        rm[rmOffset + 5] = c;
        rm[rmOffset + 10] = c;
        rm[rmOffset + 6] = s;
        rm[rmOffset + 9] = -s;
        rm[rmOffset + 1] = 0;
        rm[rmOffset + 2] = 0;
        rm[rmOffset + 4] = 0;
        rm[rmOffset + 8] = 0;
        rm[rmOffset + 0] = 1;
    }
    else if (0.0f == x && 1.0f == y && 0.0f == z)
    {
        rm[rmOffset + 0] = c;
        rm[rmOffset + 10] = c;
        rm[rmOffset + 8] = s;
        rm[rmOffset + 2] = -s;
        rm[rmOffset + 1] = 0;
        rm[rmOffset + 4] = 0;
        rm[rmOffset + 6] = 0;
        rm[rmOffset + 9] = 0;
        rm[rmOffset + 5] = 1;
    }
    else if (0.0f == x && 0.0f == y && 1.0f == z)
    {
        rm[rmOffset + 0] = c;
        rm[rmOffset + 5] = c;
        rm[rmOffset + 1] = s;
        rm[rmOffset + 4] = -s;
        rm[rmOffset + 2] = 0;
        rm[rmOffset + 6] = 0;
        rm[rmOffset + 8] = 0;
        rm[rmOffset + 9] = 0;
        rm[rmOffset + 10] = 1;
    }
    else
    {
        float len = (float)sqrt(x * x + y * y + z * z);
        if (1.0f != len)
        {
            float recipLen = 1.0f / len;
            x *= recipLen;
            y *= recipLen;
            z *= recipLen;
        }
        float nc = 1.0f - c;
        float xy = x * y;
        float yz = y * z;
        float zx = z * x;
        float xs = x * s;
        float ys = y * s;
        float zs = z * s;
        rm[rmOffset + 0] = x  * x  * nc + c;
        rm[rmOffset + 4] = xy * nc - zs;
        rm[rmOffset + 8] = zx * nc + ys;
        rm[rmOffset + 1] = xy * nc + zs;
        rm[rmOffset + 5] = y  * y  * nc + c;
        rm[rmOffset + 9] = yz * nc - xs;
        rm[rmOffset + 2] = zx * nc - ys;
        rm[rmOffset + 6] = yz * nc + xs;
        rm[rmOffset + 10] = z * z  * nc + c;
    }
    return rm;
}

void 
GLES2Render::drawFrame()
{
    //清除深度缓冲,颜色缓冲
    glClear(GL_COLOR_BUFFER_BIT);
    //选择着色程序
    glUseProgram(m_glInstance.pProgram);

    //传入变换矩阵数据
    //1.初始化旋转矩阵
    float * maMVPMatrix = getRotateM(NULL, 0, 0, 0, 0, 1);   
    //2.传矩阵数据到顶点着色器
    glUniformMatrix4fv(m_glInstance.maMVPMatrixHandle, 1, GL_FALSE, maMVPMatrix);
    //3.释放
    free(maMVPMatrix);

    //传入顶点坐标
    glVertexAttribPointer(m_glInstance.maPositionHandle,
        3,//GLint size X Y Z
        GL_FLOAT,//GLenum type
        GL_FALSE,//GLboolean normalized
        3 * 4,//GLsizei stride  dataVertex中三个数据一组
        dataVertex//const GLvoid * ptr
        );

    //传入纹理坐标
    glVertexAttribPointer(m_glInstance.maTexCoorHandle,
        2,//S T
        GL_FLOAT,//GLenum type
        GL_FALSE,//GLboolean normalized
        2 * 4,//GLsizei stride   dataTexCoor中两个数据一组
        dataTexCoor//const GLvoid * ptr
        );

    //绑定纹理
    bindTexture(GL_TEXTURE0, m_glInstance.yTexture, m_glInstance.yBuffer, m_glInstance.pWidth, m_glInstance.pHeight);
    bindTexture(GL_TEXTURE1, m_glInstance.uTexture, m_glInstance.uBuffer, m_glInstance.pWidth / 2, m_glInstance.pHeight / 2);
    bindTexture(GL_TEXTURE2, m_glInstance.vTexture, m_glInstance.vBuffer, m_glInstance.pWidth / 2, m_glInstance.pHeight / 2);


    //片元中uniform 2维均匀变量赋值
    glUniform1i(m_glInstance.myTextureHandle, 0); //对应纹理第1层
    glUniform1i(m_glInstance.muTextureHandle, 1); //对应纹理第2层
    glUniform1i(m_glInstance.mvTextureHandle, 2); //对应纹理第3层


    //enable之后这些引用才能在shader中生效
    glEnableVertexAttribArray(m_glInstance.maPositionHandle);
    glEnableVertexAttribArray(m_glInstance.maTexCoorHandle);

    //绘制 从顶点0开始绘制，总共四个顶点，组成两个三角形，两个三角形拼接成一个矩形纹理，也就是我们的画面
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


