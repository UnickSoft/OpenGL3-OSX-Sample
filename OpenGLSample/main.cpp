
#include <GL/glew.h>
#include <OpenGL/gl3.h>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <oglplus/all.hpp>
#include <oglplus/shapes/sphere.hpp>

class Example
{
public:
    
    oglplus::Context gl;
    oglplus::VertexShader vs;
    oglplus::FragmentShader fs;
    oglplus::Program prog;
    oglplus::Buffer positions;
    oglplus::Buffer colors;
    oglplus::VertexArray triangle;
    oglplus::shapes::Sphere make_sphere;
    oglplus::Buffer indexes;
    int nVertexCount;
    
    Example()
    {
        using namespace oglplus;
        
        vs.Source(
                  "#version 330\n"
                  "in vec3 Position;"
                  "in vec3 Normal;"
                  "out vec3 vertNormal;"
                  "void main(void)"
                  "{"
                  "	gl_Position = vec4(Position, 1.0);\n"
                  " vertNormal = Normal;\n"
                  "}"
                  );
        vs.Compile();
        
        fs.Source(
                  "#version 330\n"
                  "in  vec3 vertNormal;"
                  "out vec4 fragColor;"
                  "void main(void)"
                  "{\n"
                  " float dot_product = max(dot(normalize(vec3(0.5, 1.0, -1.0)), normalize(vertNormal)), 0.1);\n"
                  " fragColor = vec4(dot_product * vec3(1.0), 1.0);\n"
                  "}"
                  );
        fs.Compile();
        
        prog.AttachShader(vs);
        prog.AttachShader(fs);
        
        prog.Link();
        prog.Use();
        
        triangle.Bind();
    
        {
            std::vector<GLfloat> data;
            GLuint n_per_vertex = make_sphere.Positions(data);
            
            nVertexCount = data.size();
            
            positions.Bind(BufferTarget::Array);
            Buffer::Data(BufferTarget::Array, data);
            
            VertexArrayAttrib vert_attr(prog, "Position");
            vert_attr.Setup<GLfloat>(n_per_vertex);
            vert_attr.Enable();
        }
        
        {
            std::vector<GLfloat> data;
            GLuint n_per_vertex = make_sphere.Normals(data);
            
            // bind the VBO for the triangle colors
            colors.Bind(Buffer::Target::Array);
            // upload the data
            Buffer::Data(Buffer::Target::Array, data);
            // setup the vertex attribs array
            VertexArrayAttrib vert_attrColor(prog, "Normal");
            vert_attrColor.Setup<GLfloat>(n_per_vertex);
            vert_attrColor.Enable();
        }
        
        gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gl.ClearDepth(1.0f);
        gl.Enable(Capability::DepthTest);
        
        // Create indexes buffer.
        std::vector<GLuint> shape_indices = make_sphere.Indices();
        indexes.Bind(Buffer::Target::ElementArray);
        Buffer::Data(Buffer::Target::ElementArray, shape_indices);
    }
    
    
    void RenderFunction()
    {
        using namespace oglplus;
        
        gl.Clear().ColorBuffer().DepthBuffer();
        
        gl.DrawElements(PrimitiveType::TriangleStrip, nVertexCount, DataType::UnsignedInt);
    }
};

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(640, 480, "Glfw + Glew + OGLPlus", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        printf("Error: %s\n", glewGetErrorString(err));
        return -1;
    }
    
    // Need call after glewInit, because we can have gl error and oglplus will catch it.
    glGetError();
    
    Example example;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        example.RenderFunction();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    return 0;
}