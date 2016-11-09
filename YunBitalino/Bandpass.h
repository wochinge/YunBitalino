//Filters created by http://www.schwietering.com/jayduino/filtuino/

//Band pass bessel filter order=1 alpha1=0.003 alpha2=0.02
class  BandpassFilter
{
  public:
    BandpassFilter()
    {
      v[0]=0.0;
      v[1]=0.0;
    }
  private:
    float v[3];
  public:
    float step(float x) //class II
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = (5.408988766979797719e-2 * x)
         + (-0.89850964049624548746 * v[0])
         + (1.89625943985579858975 * v[1]);
      return
         (v[2] - v[0]);
    }
};
