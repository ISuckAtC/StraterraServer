// StraterraServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/asio.hpp>


using namespace boost::asio;
using namespace boost::asio::ip;

int readBytes(unsigned char* data, tcp::socket* socket);

int main()
{

    io_context iocontext;

    try
    {
        tcp::endpoint endpoint(tcp::v4(), 6969);

        std::cout << "START" << std::endl;

        tcp::acceptor acceptor(iocontext, endpoint);

        std::cout << "ACCEPTOR" << std::endl;

        tcp::socket socket(iocontext);

        std::cout << "SOCKET" << std::endl;

        //tcp::endpoint endp = socket.local_endpoint();

        //iocontext.run();

        std::cout << "ENDPOINT" << std::endl;

        acceptor.accept(socket);

        std::cout << "ACCEPT" << std::endl;


        unsigned char dataData[256];

        int bytesRead = readBytes(dataData, &socket);

        std::cout << "Bytes read: " << bytesRead << std::endl;

        char text[64];

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

        *in = '\0';

        std::cout << text << std::endl;

        socket.close();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}

int readBytes(unsigned char* data, tcp::socket* socket)
{
    streambuf readBuf;
    unsigned char* rBuf;
    unsigned char* out;

    int bRead = read(*socket, readBuf, transfer_exactly(4));

    std::cout << "bRead: " << bRead << std::endl;

    rBuf = (unsigned char*)(readBuf.data().data());
    out = rBuf;

    int size = (*(out) << 0 | *(out + 1) << 8 | *(out + 2) << 16 | *(out + 3) << 24);

    std::cout << "SIZE: " << size << std::endl;

    size -= 4;

    readBuf.consume(bRead);

    bRead = read(*socket, readBuf, transfer_exactly(size));

    std::cout << "bRead: " << bRead << std::endl;

    rBuf = (unsigned char*)(readBuf.data().data());
    out = rBuf;

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
