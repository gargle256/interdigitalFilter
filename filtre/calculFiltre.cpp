#include "stdafx.h"
#include "calculFiltre.h"


calculFiltre::calculFiltre()
{
}


void calculFiltre::setPath(CString fileName)
{
	m_fileName = fileName;
}

void calculFiltre::run(int elements, double ripple, double FZF_center, double F_BW, double R, double d_gnd, double diam, double end_spacing, int freq_rej, double step)
{
	m_elements = elements;            //number of elements
	m_ripple = ripple;              //ripple if cheby
	m_FZGC = FZF_center / 1000;
	m_BWMC = F_BW;
	m_R = R;						//f center in GHZ, BW in mhz, load resistance in ohms
	m_H = d_gnd;
	m_D = diam;
	m_E = end_spacing;            // Height above gnd, rod dia, end spacing
	m_NFR = freq_rej;                     //number of frequency rejection points in graph
	m_STP = step;                  //frequency step size

	Compute();
}

double calculFiltre::FuncFNRJ(double ta, double b, double c, double d)
{
	return ((b*c) - (ta*d)) / ((c*c) + (d*d));
}

void calculFiltre::cheb(int ele, double rip)
{
	double beta, gamma, c;
	int k;

	c = (2 * rip) / 17.37;
	beta = log((exp(c) + 1) / (exp(c) - 1));
	gamma = 0.5 * (exp(beta / (2 * ele)) - exp(-beta / (2 * ele)));
	for (k = 0;k<ele;k++)
	{
		m_A[k] = sin(0.5 * (2 * (k + 1) - 1) * PI / ele);
		m_B[k] = (gamma * gamma) + pow(sin((k + 1)*PI / ele), 2);
	}
	m_G[0] = 2 * m_A[0] / gamma;
	for (k = 1;k<ele;k++)
	{
		m_G[k] = 4 * m_A[k - 1] * m_A[k] / (m_B[k - 1] * m_G[k - 1]);
	}
	int nn = ele / 2;
	int nnn = (ele + 1) / 2;
	int s = nnn - nn;
	if (s <= 0)
	{
		m_G[ele] = pow(((exp(beta / 2) + 1) / (exp(beta / 2) - 1)), 2);
		return;
	}
	m_G[ele] = 1;
	return;
}

void calculFiltre::butter(int ele)
{
	const double pov2 = 1.57079633;
	int k;
	for (k = 0;k<ele;k++)
	{
		m_G[k] = 2 * sin(pov2 * (2 * (k + 1) - 1) / ele);
	}
	m_G[ele] = 1;
	return;
}

void calculFiltre::Compute(void)
{
	int k, ip, conter = 0;
	int start = -m_NFR / 2;
	int end = m_NFR / 2;
	double bw3gc, bwrgc, bw3, b, ca;
	int ele = m_elements;

	for (ip = start;ip<end;ip++)
	{
		m_FR[conter++] = m_FZGC + (m_STP * 0.001 * ip);
	}

	double F1 = m_FZGC - 0.0005 * m_BWMC;
	double F2 = m_FZGC + 0.0005 * m_BWMC;

	if (m_ripple == 0.0)
	{
		bw3gc = F2 - F1;
		bwrgc = 0;
		bw3 = 1;
		butter(ele);
	}
	else
	{
		b = 1 / sqrt(pow(10, 0.1*m_ripple) - 1);
		ca = log(b + sqrt(b * b - 1)) / ele;
		bw3 = (exp(ca) + exp(-ca)) / 2;
		cheb(ele, m_ripple);
		bwrgc = F2 - F1;
		bw3gc = bwrgc * bw3;
	}

	double w = 2 * (F2 - F1) / (F2 + F1);
	double qf = m_FZGC / bw3gc;
	int nfm = ele - 1;
	double qwvl = 11.8028 / (4 * m_FZGC);

	for (k = 0;k<nfm;k++)
	{
		m_AK[k] = 1 / (bw3 * sqrt(m_G[k] * m_G[k + 1]));
		m_RK[k] = m_AK[k] / qf;
	}

	double ako = m_G[0] * bw3;
	m_AK[ele - 1] = ako;
	m_AK[ele] = 0;
	double qs = m_G[0] * bw3 * qf;
	double canh = (exp(2 * PI*m_E / m_H) - 1) / (exp(2 * PI*m_E / m_H) + 1);
	double zm = 59.9585 * log(4 * m_H / (PI*m_D));
	double ze = 59.9585 * log(canh * m_H * 4 / (PI * m_D));
	double rkm = m_RK[0] * sqrt(zm / ze);
	double z = PI * m_D / (2 * m_H);
	double coth = (exp(z) + 1) / (exp(z) - 1);
	double y = PI * rkm / 4;
	double t = pow(coth, y);

	m_C[0] = (m_H / PI) * log((t + 1) / (t - 1));
	int mfl = ele - 2;
	for (k = 2;k <= mfl;k++)
	{
		double y = PI * m_RK[k - 1] / 4;
		double t = pow(coth, y);
		m_C[k - 1] = (m_H / PI) * log((t + 1) / (t - 1));
	}
	m_C[ele - 2] = m_C[0];
	double x = sqrt(PI * m_R / (4 * ze * qs));
	double aq = 2 * qwvl * atan(x / sqrt(1 - x * x)) / PI;
	double qu = 2200 * m_H * sqrt(m_FZGC);
	double sumg = 0;

	for (k = 0;k<ele;k++) sumg += m_G[k];
	double bloss = 4.34 * m_FZGC * sumg / (qu * (F2 - F1));
	double delay = sumg / (2 * PI * (F2 - F1));

	CFile fic;
	fic.Open(m_fileName, CFile::modeCreate | CFile::modeWrite);

	CStringA data;
	data.Format("Fiche technique pour un filtre interdigital passe bande a %d sections.\r\n", m_elements);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Bandpass ripple = %.3f dB \r\n", m_ripple);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Fréquence centrale = %.3f MHZ\r\n", m_FZGC*1000);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Fréquence de coupure = %.3f and %.3f MHZ\r\n", F1*1000, F2*1000);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Ripple BW = %.3f MHZ\r\n", bwrgc*1000);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("bande passante a 3dB = %.3f MHZ\r\n", bw3gc*1000);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Fractional BW = %.3f MHZ\r\n", w*1000);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("facteur Q du filtre = %.3f\r\n", qf);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("QU estimé = %.3f\r\n", qu);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Pertes basées sur QU = %.3fdB\r\n", bloss);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Delais au centre de la bande = %.3fns\r\n\r\n", delay);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Freq \tperte en dB\r\n");
	fic.Write(data.GetBuffer(), data.GetLength());

	int i;
	for (i = 0;i<m_NFR;i++)
	{
		double nfn = (2 * (m_FR[i] - m_FZGC)) / (w * m_FZGC);
		if (nfn < 0.0) nfn = -nfn;  //abs value of nfn

		if (m_ripple == 0.0)
		{
			m_ALOSS[i] = 10 * log((double)1 + pow(nfn, 2 * ele)) / log((double)10);
		}
		else
		{
			if (nfn < 1.0) nfn = 1.0;
			double ang = ele * log(nfn + sqrt(nfn * nfn - 1));
			double yak = 0.5 * (exp(ang) + exp(-ang));
			m_ALOSS[i] = 10 * log((double)1 + (pow(10, (0.1*m_ripple)) - 1) * yak * yak) / log((double)10);
		}
		int Aloss;
		if (m_ALOSS[i] > 65) Aloss = 65; else Aloss = (int)m_ALOSS[i];
		double fr = m_FR[i] * 1000;
		double Alos = m_ALOSS[i];

		data.Format("%.1f \t %.1f\r\n", fr, Alos);
		fic.Write(data.GetBuffer(), data.GetLength());
	}

	double wo = 2 * PI * m_FZGC * 1e+9;
	double f = m_D / m_H;
	double cf = (-0.0000422 + 0.0857397 * f + 0.0067853 * f * f - 9.092165e-2
		* pow(f, 3) + 0.169088 * pow(f, 4)) * PI * m_H * 2.54;

	double ww = wo * 1e-12;
	double b2 = PI * aq / (2 * qwvl);
	double gg = 1 / m_R;
	double bb = -cos(b2) / (ze * sin(b2));
	double el1 = 0.8 * qwvl;

	double ang = el1 * PI / (2 * qwvl);
	double b1 = ang - b2;
	double yl = -cos(ang) / (zm * sin(ang));
	double cp = ww * (cf + 0.17655 * m_D * m_D / (qwvl - el1));
	double y1 = cp + yl;
	double el2 = 0.87 * qwvl;

	ang = el2 * PI / (2 * qwvl);
	double b4 = ang - b2;
	yl = -cos(ang) / (zm * sin(ang));
	double cd = ww * (cf + 0.17655 * m_D * m_D / (qwvl - el2));
	double y2 = cd + yl;
	double el3 = 0.95 * qwvl;

	ang = el3 * PI / (2 * qwvl);
	double  b5 = ang - b2;
	yl = -cos(ang) / (zm * sin(ang));
	double  cq = ww * (cf + 0.17655 * m_D * m_D / (qwvl - el3));
	double  y3 = cq + yl;
	double elem = y3 * y2 * el1 / ((y1 - y2) * (y1 - y3)) + y1 * y3 * el2
		/ ((y2 - y1)*(y2 - y3)) + y1 * y2 * el3 / ((y3 - y1) * (y3 - y2));

	double tann = sin(b1) / cos(b1);
	yl = FuncFNRJ(gg, bb + tann / ze, 1 - ze*bb*tann, ze*gg*tann);
	y1 = cp + yl;
	tann = sin(b4) / cos(b4);
	yl = FuncFNRJ(gg, bb + tann / ze, 1 - ze*bb*tann, ze*gg*tann);
	y2 = cd + yl;
	tann = sin(b5) / cos(b5);
	yl = FuncFNRJ(gg, bb + tann / ze, 1 - ze*bb*tann, ze*gg*tann);
	y3 = cq + yl;

	double eleq = y3 * y2 * el1 / ((y1 - y2) * (y1 - y3)) + y1 * y3 * el2
		/ ((y2 - y1) * (y2 - y3)) + y1 * y2 * el3 / ((y3 - y1) * (y3 - y2));

	data.Format("\r\nlongueur du quart d'onde = %.3f cm\r\n", qwvl*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("longueur des éléments internes = %.3f cm\r\n", elem*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("longueur du dernier élément = %.3f cm\r\n", eleq*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Ground plane space = %.3f cm\r\n", m_H*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("diametre des barres = %.3f cm\r\n\r\n", m_D*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("End plates are %.3f cm from center of end rod.\r\n", m_E*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Tap external lines up %.3f cm from shorted end.\r\n", aq*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("Impedance de la ligne : dernier barre : %.3f autre : %.3f ligne ext. : %.3f ohms.\r\n", ze,zm, m_R);
	fic.Write(data.GetBuffer(), data.GetLength());

	int Tab = 10;

	data.Format("\r\nDimensions (cm)\r\n\tEl. No.\t\tEnd to C\tC to C\t\tG[k]\t\tQ/Coup\r\n");
	fic.Write(data.GetBuffer(), data.GetLength());

	double dom = m_E;
	int goo = 1;

	data.Format("\t0\r\n\t1\t\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\t\r\n", m_E*2.54, m_C[0]*2.54, m_G[0]*2.54, m_AK[0]*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	for (i = 0;i<nfm;i++)
	{
		int L = i + 1;
		dom = dom + m_C[i];
		data.Format("\t%d\t\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\t\r\n", L+1, dom*2.54, m_C[L]>0?m_C[L]*2.54:0.0, m_G[L]*2.54, m_AK[L]*2.54);
		fic.Write(data.GetBuffer(), data.GetLength());
	}

	data.Format("\t%d\t\t%.3f\r\n", ele + 1, (dom + 1)*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	data.Format("\r\ndimensions intérieure de la boite : %.3f H x %.3f W x %.3f T cm.\r\n", qwvl*2.54, (dom + m_E)*2.54, m_H*2.54);
	fic.Write(data.GetBuffer(), data.GetLength());

	fic.Close();
	MessageBox(NULL, L"Fichier résultat : " + m_fileName, L"Calcul effectué", MB_OK);
}

