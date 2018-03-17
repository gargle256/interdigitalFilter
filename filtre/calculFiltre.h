#pragma once
class calculFiltre
{
public:
	calculFiltre();
	void setPath(CString fileName);
	void run(int elements, double ripple, double FZF_center, double F_BW, double R, double d_gnd, double diam, double end_spacing, int freq_rej, double step);


private:
	double calculFiltre::FuncFNRJ(double ta, double b, double c, double d);
	void calculFiltre::cheb(int ele, double rip);
	void calculFiltre::butter(int ele);
	void calculFiltre::Compute(void);


	const double PI = 3.14159265;
	double m_G[200], m_C[200], m_RK[200], m_AK[200], m_FR[40], m_ALOSS[40], m_A[200], m_B[200];
	int m_elements;            //number of elements
	double m_ripple;              //ripple if cheby
	double m_FZGC, m_BWMC, m_R;      //f center in GHZ, BW in mhz, load resistance in ohms
	double m_H, m_D, m_E;            // Height above gnd, rod dia, end spacing
	int m_NFR;                     //number of frequency rejection points in graph
	double m_STP;                  //frequency step size
	CString m_fileName;
};

