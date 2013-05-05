#include "console.h"

void consolePrint(std::string outputString)
{
    std::cout << outputString << std::endl;
}

void consolePrint(std::string outputString, double var)
{
    std::cout << outputString << var << std::endl;
}

std::string consoleGet()
{
    std::string input;
    std::cin >> input;
    return input;
}

void errorCheck(int line, std::string filename)
{
    GLenum errCode;
    const GLubyte * errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
    errString = gluErrorString(errCode);
    std::cout << "ERROR: " << errString << ". Line: " << line << " of file: " << filename << std::endl;
    }

}

void consoleClear()
{
    std::cout << std::string(100, '\n');
}
