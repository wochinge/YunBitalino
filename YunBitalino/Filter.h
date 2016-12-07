//Filters created by http://www.schwietering.com/jayduino/filtuino/

//High pass bessel filter order=1 alpha1=0.006 frequency=15hz
class ECGFilter
{
	public:
		ECGFilter()
		{
			v[0]=0.0;
		}
	private:
		float v[2];
	public:
		float step(float x) //class II
		{
			v[0] = v[1];
			v[1] = (9.814970254751076428e-1 * x)
				 + (0.96299405095021539669 * v[0]);
			return
				 (v[1] - v[0]);
		}
};

//Low pass bessel filter order=1 alpha1=0.0019230769230769 frequency=5hz
class  RespirationFilter
{
	public:
		RespirationFilter()
		{
			v[0]=0.0;
		}
	private:
		float v[2];
	public:
		float step(float x) //class II
		{
			v[0] = v[1];
			v[1] = (5.578748699221092178e-3 * x)
				 + (0.98884250260155781564 * v[0]);
			return
				 (v[0] + v[1]);
		}
};
