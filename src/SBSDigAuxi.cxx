#include "g4sbs_types.h"
#include "SBSDigAuxi.h"
#include "TMath.h"
#define DEBUG 0

using namespace std;

bool UnfoldData(g4sbs_tree* T, double theta_sbs, double d_hcal, TRandom3* R, 
		std::vector<SBSDigPMTDet*> pmtdets, 
		std::vector<int> detmap,
		std::vector<SBSDigGEMDet*> gemdets, 
		std::vector<int> gemmap,
		//std::map<int, SBSDigPMTDet*> pmtdets, 
		//std::map<int, SBSDigGEMDet*> gemdets, 
		double tzero,
		int signal)
{
  bool has_data = false;
  
  int Npe;
  double t;
  
  double x_ref = d_hcal*sin(theta_sbs);
  double z_ref = d_hcal*cos(theta_sbs);
  
  double z_hit, Npe_Edep_ratio, sigma_tgen;

  //double z_det, pz, E, 
  double beta, sin2thetaC;
  
  int chan;
  int mod;
  
  int idet = 0;
  //ordering by increasing unique det ID
  while(detmap[idet]!=HCAL_UNIQUE_DETID && idet<detmap.size())idet++;
  if(idet>=detmap.size())idet = -1;
  //cout << " " << idet;  
  if(idet>=0 && T->Harm_HCalScint.sumedep) {
    for(size_t k = 0; k < T->Harm_HCalScint.sumedep->size(); k++) {
      chan = T->Harm_HCalScint.cell->at(k);
      //T->Harm_HCalScint_hit_sumedep->at(k);
      
      z_hit = -(T->Harm_HCalScint.xhitg->at(k)-x_ref)*sin(theta_sbs)+(T->Harm_HCalScint.zhitg->at(k)-z_ref)*cos(theta_sbs);
      
      // Evaluation of number of photoelectrons from energy deposit documented at:
      // https://sbs.jlab.org/DocDB/0000/000043/002/Harm_HCal_Digi_EdepOnly_2.pdf
      // TODO: put that stuff in DB...
      Npe_Edep_ratio = 5.242+11.39*z_hit+10.41*pow(z_hit, 2);
      Npe = R->Poisson(Npe_Edep_ratio*T->Harm_HCalScint.sumedep->at(k)*1.0e3);
      t = tzero+R->Gaus(T->Harm_HCalScint.tavg->at(k)+10.11, 1.912)-pmtdets[idet]->fTrigOffset;
      
      sigma_tgen = 0.4244+11380/pow(Npe+153.4, 2);
      //Generate here,...
      //R->Landau(t, sigma_tgen);
      //if(chan>pmtdets[idet]->fNChan)cout << chan << endl;
      pmtdets[idet]->PMTmap[chan].Fill(Npe, pmtdets[idet]->fThreshold, t, sigma_tgen, signal);
    }
    has_data = true;
  }
  
  while(detmap[idet]!=BBPS_UNIQUE_DETID && idet<detmap.size())idet++;
  if(idet>=detmap.size())idet = -1;
  //cout << " " << idet;  
  // Process BB PS data
  if(idet>=0 && T->Earm_BBPSTF1.nhits){
    for(int i = 0; i<T->Earm_BBPSTF1.nhits; i++){
      // Evaluation of number of photoelectrons and time from energy deposit documented at:
      if(T->Earm_BBPSTF1.sumedep->at(i)<1.e-4)continue;
      //check probability to generate p.e. yield
      bool genpeyield = true;
      if(T->Earm_BBPSTF1.sumedep->at(i)<1.e-2)
	genpeyield = R->Uniform(0, 1)<=1-exp(0.29-950.*T->Earm_BBPSTF1.sumedep->at(i));
      //if we're go, let's generate
      if(genpeyield){
	beta = sqrt( pow(m_e+T->Earm_BBPSTF1.sumedep->at(i), 2)-m_e*m_e )/(m_e + T->Earm_BBPSTF1.sumedep->at(i));
	sin2thetaC = TMath::Max(1.-1./pow(n_lg*beta, 2), 0.);
	//1500. Used to be 454.: just wrong
	Npe = R->Poisson(300.0*T->Earm_BBPSTF1.sumedep->at(i)*sin2thetaC/(1.-1./(n_lg*n_lg)) );
	t = tzero+T->Earm_BBPSTF1.tavg->at(i)+R->Gaus(3.2-5.805*T->Earm_BBPSTF1.zhit->at(i)-17.77*pow(T->Earm_BBPSTF1.zhit->at(i), 2), 0.5)-pmtdets[idet]->fTrigOffset;
	chan = T->Earm_BBPSTF1.cell->at(i);
	//T->Earm_BBPSTF1_hit_sumedep->at(i);
	
	//if(chan>pmtdets[idet]->fNChan)cout << chan << endl;
	pmtdets[idet]->PMTmap[chan].Fill(pmtdets[idet]->fRefPulse, Npe, 0, t, signal);
      }
    }
    has_data = true;
  }
  
  while(detmap[idet]!=BBSH_UNIQUE_DETID && idet<detmap.size())idet++;
  if(idet>=detmap.size())idet = -1;
  //cout << " " << idet;
  if(idet>=0 && T->Earm_BBSHTF1.nhits){
    for(int i = 0; i<T->Earm_BBSHTF1.nhits; i++){
      // Evaluation of number of photoelectrons and time from energy deposit documented at:
      // 
       if(T->Earm_BBSHTF1.sumedep->at(i)<1.e-4)continue;
      //check probability to generate p.e. yield
      bool genpeyield = true;
      if(T->Earm_BBSHTF1.sumedep->at(i)<1.e-2)genpeyield = R->Uniform(0, 1)<=1-exp(0.29-950.*T->Earm_BBSHTF1.sumedep->at(i));
      //if we're go, let's generate
      if(genpeyield){
	beta = sqrt( pow(m_e+T->Earm_BBSHTF1.sumedep->at(i), 2)-m_e*m_e )/(m_e + T->Earm_BBSHTF1.sumedep->at(i));
	sin2thetaC = TMath::Max(1.-1./pow(n_lg*beta, 2), 0.);
	//1800. Used to be 932.: just wrong
	Npe = R->Poisson(360.0*T->Earm_BBSHTF1.sumedep->at(i)*sin2thetaC/(1.-1./(n_lg*n_lg)) );
	t = tzero+T->Earm_BBSHTF1.tavg->at(i)+R->Gaus(2.216-8.601*T->Earm_BBSHTF1.zhit->at(i)-7.469*pow(T->Earm_BBSHTF1.zhit->at(i), 2), 0.8)-pmtdets[idet]->fTrigOffset;
	chan = T->Earm_BBSHTF1.cell->at(i);
	//T->Earm_BBSHTF1_hit_sumedep->at(i);
		
	//if(chan>pmtdets[idet]->fNChan)cout << chan << endl;
	pmtdets[idet]->PMTmap[chan].Fill(pmtdets[idet]->fRefPulse, Npe, 0, t, signal);
      }
    }
    has_data = true;
  }

  while(detmap[idet]!=GRINCH_UNIQUE_DETID && idet<detmap.size())idet++;
  if(idet>=detmap.size())idet = -1;
  //cout << " " << idet;  
  // Process GRINCH data
  if(idet>=0 && T->Earm_GRINCH.nhits){
    for(int i = 0; i<T->Earm_GRINCH.nhits; i++){
      chan = int(T->Earm_GRINCH.PMT->at(i)/5)-1;
      t = tzero+T->Earm_GRINCH.Time_avg->at(i)+pmtdets[idet]->fTrigOffset;
      Npe = T->Earm_GRINCH.NumPhotoelectrons->at(i);
      
      //if(chan>pmtdets[idet]->fNChan)cout << chan << endl;
      pmtdets[idet]->PMTmap[chan].Fill(pmtdets[idet]->fRefPulse, Npe, pmtdets[idet]->fThreshold, t, signal);
    }
    has_data = true;
  }
  
  while(detmap[idet]!=HODO_UNIQUE_DETID && idet<detmap.size())idet++;
  if(idet>=detmap.size())idet = -1;
  //cout << " " << idet;
  // Process hodoscope data
  if(idet>=0 && T->Earm_BBHodoScint.nhits){
    for(int i = 0; i<T->Earm_BBHodoScint.nhits; i++){
      for(int j = 0; j<2; j++){//j = 0: close beam PMT, j = 1: far beam PMT
	// Evaluation of number of photoelectrons and time from energy deposit documented at:
	// https://hallaweb.jlab.org/dvcslog/SBS/170711_172759/BB_hodoscope_restudy_update_20170711.pdf
	Npe = R->Poisson(1.0e7*T->Earm_BBHodoScint.sumedep->at(i)*0.113187*exp(-(0.3+pow(-1, j)*T->Earm_BBHodoScint.xhit->at(i))/1.03533)* 0.24);
	t = tzero+T->Earm_BBHodoScint.tavg->at(i)+(0.55+pow(-1, j)*T->Earm_BBHodoScint.xhit->at(i))/0.15-pmtdets[idet]->fTrigOffset;
	chan = T->Earm_BBHodoScint.cell->at(i)*2+j;
	//T->Earm_BBHodoScint_hit_sumedep->at(i);
	//if(chan>pmtdets[idet]->fNChan)cout << chan << endl;
	pmtdets[idet]->PMTmap[chan].Fill(pmtdets[idet]->fRefPulse, Npe, pmtdets[idet]->fThreshold, t, signal);
      }
    }
    has_data = true;
  }
  
  idet = 0;
  while(gemmap[idet]!=BBGEM_UNIQUE_DETID && idet<gemmap.size())idet++;
  if(idet>=gemmap.size())idet = -1;
  //cout << " gem " << idet << endl;
  // Now process the GEM data
  if(idet>=0 && T->Earm_BBGEM.nhits){
    for(int k = 0; k<T->Earm_BBGEM.nhits; k++){
      if(T->Earm_BBGEM.edep->at(k)>0){
	SBSDigGEMDet::gemhit hit; 
	hit.source = signal;
	if(T->Earm_BBGEM.plane->at(k)==5){
	  if(fabs(T->Earm_BBGEM.xin->at(k))>=1.024)continue;
	  mod = 12 + floor((T->Earm_BBGEM.xin->at(k)+1.024)/0.512);
	}else{
	  if(fabs(T->Earm_BBGEM.xin->at(k))>=0.768)continue;
	  mod = (T->Earm_BBGEM.plane->at(k)-1)*3 + floor((T->Earm_BBGEM.xin->at(k)+0.768)/0.512);
	}
	hit.module = mod; 
	hit.edep = T->Earm_BBGEM.edep->at(k)*1.0e9;//eV! not MeV!!!!
	//hit.tmin = T->Earm_BBGEM_hit_tmin->at(k);
	//hit.tmax = T->Earm_BBGEM_hit_tmax->at(k);
	hit.t = tzero+T->Earm_BBGEM.t->at(k);
	//cout << mod*2 << " " << gemdets[idet]->GEMPlanes[mod*2].Xoffset() << endl;
	hit.xin = T->Earm_BBGEM.xin->at(k)-gemdets[idet]->GEMPlanes[mod*2].Xoffset();
	hit.yin = T->Earm_BBGEM.yin->at(k);
	hit.zin = T->Earm_BBGEM.zin->at(k)-bbgem_z[T->Earm_BBGEM.plane->at(k)-1]+0.8031825;
	hit.xout = T->Earm_BBGEM.xout->at(k)-gemdets[idet]->GEMPlanes[mod*2].Xoffset();
	hit.yout = T->Earm_BBGEM.yout->at(k);
	hit.zout = T->Earm_BBGEM.zout->at(k)-bbgem_z[T->Earm_BBGEM.plane->at(k)-1]+0.8031825;
	
	//cout << mod << " " << hit.xin << " " << hit.xout << endl;
	gemdets[idet]->fGEMhits.push_back(hit);
	//gemhit->SetData(0,fSource);
	//gemhit->SetData(1,	T->Earm_BBGEM_hit_plane->at(k);//);
	//gemhit->SetData(2,T->Earm_BBGEM_hit_strip->at(k));
	//gemhit->SetData(3,T->Earm_BBGEM_hit_x->at(k));
	//gemhit->SetData(4,T->Earm_BBGEM_hit_y->at(k));
	//gemhit->SetData(5,T->Earm_BBGEM_hit_z->at(k));
	//gemhit->SetData(6,T->Earm_BBGEM_hit_polx->at(k));
	//gemhit->SetData(7,T->Earm_BBGEM_hit_poly->at(k));
	//gemhit->SetData(8,T->Earm_BBGEM_hit_polz->at(k));
	//gemhit->SetData(9,T->Earm_BBGEM_hit_t->at(k));
	//gemhit->SetData(10,T->Earm_BBGEM_hit_trms->at(k));
	//gemhit->SetData(11,	T->Earm_BBGEM_hit_tmin->at(k);//);
	//gemhit->SetData(12,	T->Earm_BBGEM_hit_tmax->at(k);//);
	//gemhit->SetData(13,	T->Earm_BBGEM_hit_tx->at(k);//);
	//gemhit->SetData(14,	T->Earm_BBGEM_hit_ty->at(k);//);
	//gemhit->SetData(15,	T->Earm_BBGEM_hit_txp->at(k);//);
	//gemhit->SetData(16,	T->Earm_BBGEM_hit_typ->at(k);//);
	//gemhit->SetData(17,T->Earm_BBGEM_hit_xg->at(k));
	//gemhit->SetData(18,T->Earm_BBGEM_hit_yg->at(k));
	//gemhit->SetData(19,T->Earm_BBGEM_hit_zg->at(k));
	//gemhit->SetData(20,	T->Earm_BBGEM_hit_trid->at(k);//);
	//gemhit->SetData(21,	T->Earm_BBGEM_hit_mid->at(k)+1;//);
	//gemhit->SetData(22,	T->Earm_BBGEM_hit_pid->at(k);//);
	//gemhit->SetData(23,	T->Earm_BBGEM_hit_vx->at(k);//);
	//gemhit->SetData(24,	T->Earm_BBGEM_hit_vy->at(k);//);
	//gemhit->SetData(25,	T->Earm_BBGEM_hit_vz->at(k);//);
	//gemhit->SetData(26,	T->Earm_BBGEM_hit_p->at(k);//);
	//gemhit->SetData(27,	T->Earm_BBGEM_hit_edep->at(k)*1.0e3;//); // convert to MeV?
	//gemhit->SetData(28,T->Earm_BBGEM_hit_beta->at(k));
	//gemhit->SetData(29,	T->Earm_BBGEM_hit_xin->at(k);//);
	//gemhit->SetData(30,	T->Earm_BBGEM_hit_yin->at(k);//);
	//gemhit->SetData(31,	T->Earm_BBGEM_hit_zin->at(k);//);
	//gemhit->SetData(32,	T->Earm_BBGEM_hit_xout->at(k);//);
	//gemhit->SetData(33,	T->Earm_BBGEM_hit_yout->at(k);//);
	//gemhit->SetData(34,	T->Earm_BBGEM_hit_zout->at(k);//);
	//fg4sbsHitData.push_back(gemhit);
      }//end if(sumedep>0)
      
    }
    has_data = true;  
  }
  
  return has_data;
}

