#ifndef ModelSpace_h
#define ModelSpace_h 1

//#define ARMA_NO_DEBUG
#include <vector>
#include <unordered_map>
#include <map>
#include <armadillo>
#ifndef SQRT2
  #define SQRT2 1.4142135623730950488
#endif


using namespace std;

class ModelSpace; //forward declaration so Ket can have a pointer to ModelSpace

class Orbit
{
 public:
   // Fields
   int n;
   int l;
   int j2;
   int tz2;
   int ph; // particle=0, hole=1
   int io; // inside=0, outside=1
   double spe;

   //Constructors
   Orbit();
   Orbit(int n ,int l, int j, int t, int ph, int io, double spe);
   Orbit(const Orbit&);
   // Methods
   void Set(int n, int l, int j2, int tz2, int ph, int io, double spe);
};




class Ket  //  | pq >
{
 public:
   // Fields
   int p;
   int q;
   int parity;
   int Tz;
   int Jmin;
   int Jmax;
   int Jstep;
   int E2;
   // Constructors
   Ket(){};
   Ket(ModelSpace * ms);
   Ket(ModelSpace * ms, int p, int q);
   void Setpq(int p, int q);
   // Methods
   int Phase(int J);
   int delta_pq(){return dpq;};
   ModelSpace * ms;

 private:
   // Fields
   int dpq;
   int phase_prefactor;

};




class TwoBodyChannel
{
 public:
   //Fields
   int J;
   int parity;
   int Tz;
   arma::mat Proj_pp; // Projector onto pp kets
   arma::mat Proj_hh; // Projector onto hh kets
   arma::mat Proj_ph_cc; // Projector onto ph kets, for use with cross-coupled TBMEs

   // Constructors
   TwoBodyChannel();
   TwoBodyChannel(int j, int p, int t, ModelSpace* ms);
   TwoBodyChannel(int N, ModelSpace* ms);
   TwoBodyChannel(const TwoBodyChannel& rhs) {Copy(rhs);};
   void Initialize(int N, ModelSpace* ms);

   //Overloaded operators
   TwoBodyChannel& operator=(const TwoBodyChannel& rhs) {Copy(rhs); return *this;};

   //Methods
   int GetNumberKets() const {return NumberKets;};
   int GetLocalIndex(int ketindex) const { return KetMap[ketindex];}; // modelspace ket index => local ket index
   int GetLocalIndex(int p, int q) const ;
   int GetKetIndex(int i) const { return KetList[i];}; // local ket index => modelspace ket index
   Ket & GetKet(int i) const ; // get pointer to ket using local index


//   arma::uvec GetKetIndexFromList(vector<unsigned int>& vec_in);
   arma::uvec GetKetIndexFromList(vector<long long unsigned int>& vec_in);
   arma::uvec GetKetIndex_pp();
   arma::uvec GetKetIndex_hh();
   arma::uvec GetKetIndex_ph();
   arma::uvec GetKetIndex_vv();
   arma::uvec GetKetIndex_holeq_holeq(); 
   arma::uvec GetKetIndex_particleq_particleq();
   arma::uvec GetKetIndex_particleq_holeq();
   arma::uvec GetKetIndex_v_holeq(); // added
   arma::uvec GetKetIndex_v_particleq(); //added

// private:
   //Fields
   ModelSpace * modelspace;
   int NumberKets;  // Number of pq configs that participate in this channel
   vector<int> KetList; // eg [2, 4, 7, ...] Used for looping over all the kets in the channel
   vector<int> KetMap;  // eg [ -1, -1, 0, -1, 1, -1, -1, 2 ...] Used for asking what is the local index of this ket. -1 means the ket doesn't participate in this channel
   //Methods
   virtual bool CheckChannel_ket(int p, int q) const;  // check if |pq> participates in this channel
   bool CheckChannel_ket(Ket &ket) const {return CheckChannel_ket(ket.p,ket.q);};  // check if |pq> participates in this channel
   void Copy(const TwoBodyChannel &);
   
};



class TwoBodyChannel_CC : public TwoBodyChannel
{
  public:
   TwoBodyChannel_CC();
   TwoBodyChannel_CC(int j, int p, int t, ModelSpace* ms);
   TwoBodyChannel_CC(int N, ModelSpace* ms);
   TwoBodyChannel_CC(const TwoBodyChannel& rhs) {Copy(rhs);};
   bool CheckChannel_ket(int p, int q) const;  // check if |pq> participates in this channel
};





class ThreeBodyChannel
{
 public:
   //Fields
   int Jpq;
   int J;
   int parity;
   int Tz;

   // Constructors
   ThreeBodyChannel();
   ThreeBodyChannel(int jpq, int j, int p, int t, ModelSpace* ms);
   ThreeBodyChannel(int N, ModelSpace* ms);
   ThreeBodyChannel(const ThreeBodyChannel& rhs) {Copy(rhs);};
   void Initialize(int N, ModelSpace* ms);

   //Overloaded operators
   ThreeBodyChannel& operator=(const ThreeBodyChannel& rhs) {Copy(rhs); return *this;};

   //Methods
   int GetNumberKets() const {return NumberKets;} ;
   int GetLocalIndex(int ketindex) const { return KetMap[ketindex];} ; // modelspace ket index => local ket index
   int GetLocalIndex(int p, int q, int r) const ;
   int GetKetIndex(int i) const { return KetList[i];} ; // local ket index => modelspace ket index
   Ket * GetKet(int i) const ; // get pointer to ket using local index


// private:
   //Fields
   ModelSpace * modelspace;
   int NumberKets;  // Number of pqr configs that participate in this channel
   vector<int> KetList; // eg [2, 4, 7, ...] Used for looping over all the kets in the channel
   vector<int> KetMap;  // eg [ -1, -1, 0, -1, 1, -1, -1, 2 ...] Used for asking what is the local index of this ket. -1 means the ket doesn't participate in this channel
   //Methods
   virtual bool CheckChannel_ket(int p, int q, int r) const;  // check if |pqr> participates in this channel
//   bool CheckChannel_ket(Ket3 *ket) const {return CheckChannel_ket(ket->p,ket->q,ket->r);};  // check if |pqr> participates in this channel
   void Copy(const ThreeBodyChannel &);
   
};





class ModelSpace
{

 public:


   // Constructors
   ModelSpace();
   ModelSpace(const ModelSpace&); // copy constructor
   ModelSpace( ModelSpace&&); // move constructor
   ModelSpace(int Nmax, vector<string> hole_list, vector<string> inside_list);
   ModelSpace(int Nmax, string);

   // Overloaded operators
   ModelSpace operator=(const ModelSpace&); 
   ModelSpace operator=(ModelSpace&&); 

   void Init(int Nmax, vector<string> hole_list, vector<string> inside_list);
   void Init_Skeleton(int nmax);
   void Init_He4(int nmax);
   void Init_O16(int nmax);
   void Init_Ca40(int nmax);
   void Init_PShell(int nmax);
   void Init_SDShell(int nmax);
   void Init_PSDShell(int nmax);
   void Init_O16PSDShell(int nmax);
   void Init_FPShell(int nmax);
   void Init_SDFPShell(int nmax);



   // Methods
   void SetupKets();
   void AddOrbit(Orbit orb);
   // Setter/Getters
   Orbit& GetOrbit(int i) const {return (Orbit&) Orbits[i];}; 
   Ket& GetKet(int i) const {return (Ket&) Kets[i];};
   Ket& GetKet(int p, int q) const {return (Ket&) Kets[Index2(p,q)];};
   int GetOrbitIndex(int n, int l, int j2, int tz2) const {return Index1(n,l,j2,tz2);};
   int GetKetIndex(int p, int q) const {return Index2(p,q);}; // convention is p<=q
   int GetKetIndex(Ket * ket) const {return Index2(ket->p,ket->q);}; // convention is p<=q
   int GetNumberOrbits() const {return norbits;};
   int GetNumberKets() const {return Kets.size();};
   void SetHbarOmega(double hw) {hbar_omega = hw;};
   void SetTargetMass(int A) {target_mass = A;};
   double GetHbarOmega() const {return hbar_omega;};
   int GetTargetMass() const {return target_mass;};
   int GetNumberTwoBodyChannels() const {return TwoBodyChannels.size();};
   TwoBodyChannel& GetTwoBodyChannel(int ch) const {return (TwoBodyChannel&) TwoBodyChannels[ch];};
   TwoBodyChannel_CC& GetTwoBodyChannel_CC(int ch) const {return (TwoBodyChannel_CC&) TwoBodyChannels_CC[ch];};
   inline int GetTwoBodyJmax() const {return TwoBodyJmax;};
   inline int GetThreeBodyJmax() const {return ThreeBodyJmax;};

   int GetNmax(){return Nmax;};
   int GetN2max(){return N2max;};
   int GetN3max(){return N3max;};
   void SetNmax(int n){Nmax=n;};
   void SetN2max(int n){N2max=n;};
   void SetN3max(int n){N3max=n;};

   double GetSixJ(double j1, double j2, double j3, double J1, double J2, double J3);
   double GetNineJ(double j1, double j2, double j3, double j4, double j5, double j6, double j7, double j8, double j9);
   double GetMoshinsky( int N, int Lam, int n, int lam, int n1, int l1, int n2, int l2, int L); // Inconsistent notation. Not ideal.
   bool SixJ_is_empty(){ return SixJList.empty(); };

   int GetTwoBodyChannelIndex(int j, int p, int t);
   inline int phase(int x) {return (x%2)==0 ? 1 : -1;};
   inline int phase(double x) {return phase(int(x));};

   int Index1(int n, int l, int j2, int tz2) const {return(2*n+l)*(2*n+l+3) + 1-j2 + (tz2+1)/2 ;};
   inline int Index2(int p, int q) const {return q*(q+1)/2 + p;};

   void PreCalculateMoshinsky();



   vector<long long unsigned int> holes;
   vector<long long unsigned int> particles;
   vector<long long unsigned int> valence;
   vector<long long unsigned int> qspace;
   vector<long long unsigned int> hole_qspace;
   vector<long long unsigned int> particle_qspace;
   vector<long long unsigned int> proton_orbits;
   vector<long long unsigned int> neutron_orbits;

   vector<long long unsigned int> KetIndex_pp; 
   vector<long long unsigned int> KetIndex_ph;
   vector<long long unsigned int> KetIndex_hh;
   vector<long long unsigned int> KetIndex_vv;
   vector<long long unsigned int> KetIndex_holeq_holeq; 
   vector<long long unsigned int> KetIndex_particleq_particleq;
   vector<long long unsigned int> KetIndex_particleq_holeq;
   vector<long long unsigned int> KetIndex_v_holeq; // added
   vector<long long unsigned int> KetIndex_v_particleq; //added

   int Nmax;
   int N2max;
   int N3max;
   int OneBodyJmax;
   int TwoBodyJmax;
   int ThreeBodyJmax;
//   map<array<int,3>,vector<unsigned int> > OneBodyChannels;
   map<array<int,3>,vector<long long unsigned int> > OneBodyChannels;



 private:
   // Fields
   int norbits;
   double hbar_omega;
   int target_mass;
   int nTwoBodyChannels;
   vector<Orbit> Orbits;
   vector<Ket> Kets;
   vector<TwoBodyChannel> TwoBodyChannels;
   vector<TwoBodyChannel_CC> TwoBodyChannels_CC;
//   map<long int,double> SixJList;

   unordered_map<unsigned long int,double> SixJList;
   unordered_map<unsigned long long int,double> NineJList;
   unordered_map<unsigned long long int,double> MoshList;

};




#endif

