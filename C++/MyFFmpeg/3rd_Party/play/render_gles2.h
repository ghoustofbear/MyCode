/**
 * @file      gl_render.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    shlstarfire@163.com
 * @date      2018-09-14 10:45:16
 *   
 * @note
 *  gl_render.h defines 
 */
#ifndef __GL_RENDER_GLES2_H__
#define __GL_RENDER_GLES2_H__
#include "render.h"


#include <GLES2/gl2platform.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>



class GLES2Render : public Render
{
public:
    GLES2Render(HWND wnd, bool useTxt = true);
    virtual ~GLES2Render();
    int init();
    int changeWndSize(int width, int height);
    void onYUV(void *data, int size, int w, int h);
    void onRGB(void *data, int size, int w, int h);

private:
    unsigned int buildShader(const char* source, GLenum shaderType);
    unsigned int buildProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
    void checkGlError(const char* op);
    GLuint bindTexture(GLenum texture_n, GLuint texture_id, const void *buffer, GLuint w, GLuint h);
    void drawFrame();
   

    bool m_useTxt;
    char* m_buffer;
    
    //////////////////////////
    struct Instance
    {
        //������ɫ��λ����������
        unsigned int maPositionHandle;
        //������ɫ��������������
        unsigned int maTexCoorHandle;
        //��ɫ����������
        unsigned int pProgram;
        //������ɫ�����ձ仯��������
        unsigned int maMVPMatrixHandle;
        //ƬԪ��ɫ������������
        unsigned int myTextureHandle;
        unsigned int muTextureHandle;
        unsigned int mvTextureHandle;
        //��������
        unsigned int yTexture;
        unsigned int uTexture;
        unsigned int vTexture;
        //��ɫ����Ⱦ���
        unsigned int pWidth;
        unsigned int pHeight;
        //��Ļ�Ŀ��
        unsigned int vWidth;
        unsigned int vHeight;
        //yuv����
        char *yBuffer;
        unsigned long yBufferSize;
        char *uBuffer;
        unsigned long uBufferSize;
        char *vBuffer;
        unsigned long vBufferSize;
    };

    Instance m_glInstance;
    //egl
    EGLDisplay m_EGLDisplay;
    EGLConfig  m_EGLConfig;
    EGLSurface m_EGLSurface;
    EGLContext m_EGLContext;
    //EGLNativeWindowType m_EGLWnd;


    int gles_init();

    
};
#endif
