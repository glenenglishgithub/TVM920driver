#ifndef MACHLOCH
#define MACHLOCH

#include <stdint.h>


template <class T>
class Cmachloc {
public:

	T x;
    T y;
    T * z;
    T * a;
    int N;
public :
    Cmachloc(int n) {
        x=0;
        y=0;
        z = new T[n];
        a = new T[n];
        int x=n;
        while (x--) z[x] = 0;
        x=n;
        while (x--) a[x] = 0;
        N=n;

    }


    ~Cmachloc() { delete [] z; delete [] a;}
#if 1
    void operator=(const Cmachloc<T> &  t )  {

    	x = t.x;
    	y = t.y;

        int n = N;
        while (n--)  {
            a[n] = t.a[n];
            z[n] = t.z[n];
        }

    }


#endif
    bool operator == ( const Cmachloc<T> &  t)  {
    	 bool b = ((t.x==x) && (t.y==y));
         int n=N;
         while (n--) {
             b &= (t.a[n] == a[n]);
             b &= (t.z[n] == z[n]);
         }
         return b;
    }

    bool operator != ( const Cmachloc<T> &  t)  {
    	return !(this->operator== (t));
    }

    bool ChkEqual( const Cmachloc<T> &  t1) {
    	 uint32_t b = ((t1.x==x) && (t1.y==y));
         int n=N;
         while (n--) {
             b &= (t1.a[n] == a[n]);
             b &= (t1.z[n] == z[n]);
         }
         return b;
    }

    void CalcABSDiffs(  const Cmachloc<T> &  t1,  const Cmachloc<T> &  t2) {
        /* return the ABS different t2 - t1 */
        x = fabs(t2.x - t1.x);
        y = fabs(t2.y - t1.y);
        for (int i=0;i< N ; i++) {
            z[i] = fabs(t2.z[i] - t1.z[i]);
            a[i] = fabs(t2.a[i] - t1.a[i]);
        }

    }

    bool CompareWithLimit( const Cmachloc<T> &  t1, T limit) {
		if (fabs(x - t1.x)> limit) return 0;
		if (fabs(y - t1.y)> limit) return 0;

        for (int i=0;i< N ; i++) {
            if( fabs(z[i] - t1.z[i]) > limit) return 0;
            if(  fabs(a[i] - t1.a[i]) > limit) return 0;
        }
        return true;

    }






};

#endif

