
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "ul_rx.h"

#define PNSEQLEN 2000
#define PKTLEN 640
#define ULSEQLEN 160

bool get_pnsequence();

using namespace win;

double freq = 2e9;
double sample_rate = 10e6;
double rx_gain = 20;
//double rx_gain = 30;
double amp = 0.1;
std::string device_addr = "addr=192.168.10.2";

char pnseq[PNSEQLEN];
unsigned int pnoffset;

int main(int argc, char * argv[]){

    if (argc<2)
    {
        std::cout << "Not enough arguments." << std::endl;
        return(-1);
    }
    else
    {
        pnoffset = strtol(argv[1], NULL, 10);
    }

	std::cout << "Start transmit chain..." << std::endl;
    srand(time(NULL)); //Initialize random seed

    get_pnsequence();
    ul_rx rx = ul_rx();

    std::vector<unsigned char> payload(PKTLEN,0);

    for(int i = 0; i < ULSEQLEN; i++)
    {
        payload[i] = pnseq[pnoffset + i];
    }

    std::vector<std::complex<double> > samples(payload.size());
    for (int i = 0; i<payload.size(); i++)
    {
        // samples.push_back(0.5, 0.0);
        samples[i] = amp*std::complex<double>((2*(double)payload[i])-1, 0.0);
    }

    rx.init_usrp();
    rx.use_external_clock();
    rx.reset_usrp_time();

    int num_of_samples = 2000;
    std::vector<std::complex<double> > m_samples; //!< Vector to hold the raw samples received from the USRP and passed into the ul_receiver_chain

    int rx_count = 0;
    double rx_time = 0.0;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    double curr_time = rx.get_usrp_time().get_real_secs();
    rx_time = curr_time + 0.5;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    while(1)
    {
        rx.recv_data(num_of_samples, m_samples);
    }
}


/*!
 * \Read the PN sequence from the file generated by MATLAB
 */
bool get_pnsequence()
{
    std::ifstream pnfile; 
    pnfile.open("../pnseq.dat", std::ios::in | std::ios::binary);
    if(!pnfile) 
    {
        std::cout << "Cannot open file.\n";
        return false;
    }
    pnfile.read(pnseq, PNSEQLEN);

    for(int i; i<100; i++)
        std::cout << (int)pnseq[i] << " ";
    std::cout << std::endl;
}