// StraterraServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/asio.hpp>
#include <thread>


using namespace boost::asio;
using namespace boost::asio::ip;

int readBytes(unsigned char* data, tcp::socket* socket, io_context* iocontext);

void handleSocket(tcp::socket* socket, int threadId, io_context* iocontext);

void handleRead(const boost::system::error_code& err, std::size_t);

int main()
{

    io_context iocontext;
    int threadId = 0;

    try
    {
        tcp::endpoint endpoint(tcp::v4(), 6969);

        std::cout << "START" << std::endl;

        tcp::acceptor acceptor(iocontext, endpoint);

        std::cout << "ACCEPTOR" << std::endl;


        for (;;)
        {
            tcp::socket socket(iocontext);

            std::cout << "SOCKET" << std::endl;

            acceptor.accept(socket);
            
            std::cout << "HANDLE: " << &socket << std::endl;

            std::cout << "ACCEPT" << std::endl;

            std::thread(handleSocket, &socket, threadId, &iocontext).detach();
            threadId++;
            
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}

void handleRead(const boost::system::error_code& err, std::size_t)
{

}

void handleSocket(tcp::socket* socket, int threadId, io_context* iocontext)
{
    try
    {
        std::cout << "[" << threadId << "]" << "HANDLING SOCKET" << std::endl;
        unsigned char dataData[256];
        int bytesRead;
        char text[256];

        std::cout << "[" << threadId << "]" << "HANDLE: " << socket << std::endl;

        for (;;)
        {
            std::cout << "[" << threadId << "]" << "READING" << std::endl;
            bytesRead = readBytes(dataData, socket, iocontext);

            std::cout << "Bytes read: " << bytesRead << std::endl;

            char* in = text;
            unsigned char* out = dataData;

            std::cout << "BEGIN MESSAGE: \"";

            for (int i = 0; i < bytesRead; ++i)
            {
                std::cout << std::hex << (int)*out;
                *in = (char)*out;
                in++;
                out++;
            }
            std::cout << "\"" << std::endl;

            if (text == "end")
            {
                break;
            }

            *in = '\0';

            std::cout << text << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[" << threadId << "]" << ex.what() << std::endl;
    }
    (*socket).close();
}

int readBytes(unsigned char* data, tcp::socket* socket, io_context* iocontext)
{
    streambuf readBuf;
    unsigned char* rBuf;
    unsigned char* out;
    int bRead = 0;

    int* bReadP = &bRead;

    unsigned char buf[256];

    async_read(*socket, buffer(buf, 256), transfer_exactly(4), [bReadP](const boost::system::error_code& err, std::size_t size) {
        std::cout << "Async got " << size << " bytes" << std::endl;
        std::cout << "Read result: " << err << " - " << err.message() << std::endl;
        *bReadP = size;
        });

    (*iocontext).run();

    std::cout << "bRead: " << bRead << std::endl;

    out = buf;

    int size = (*(out) << 0 | *(out + 1) << 8 | *(out + 2) << 16 | *(out + 3) << 24);

    std::cout << "SIZE: " << size << std::endl;

    size -= 4;

    async_read(*socket, buffer(buf, 256), transfer_exactly(size), [bReadP](const boost::system::error_code& err, std::size_t size) {
        std::cout << "Async got " << size << " bytes" << std::endl;
        *bReadP = size;
        });

    std::cout << "bRead: " << bRead << std::endl;

    out = buf;

    unsigned char* in = data;
    
    for (int i = 0; i < bRead; ++i)
    {
        *in++ = *out++;
    }

    return bRead;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
