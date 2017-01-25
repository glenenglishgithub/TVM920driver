/* bitvector32 optimized for arm processor */
#ifndef CBV32H
#define CBV32H

#include <assert.h>
#include <stdint.h>
#define CBV_DWORDBITS 32

typedef uint32_t cbv32native_t;

class Cbv32 {
public :
	cbv32native_t * bvptrNV;	/* where the pointer starts at - the base of the storage array*/
	cbv32native_t * bvptr; /* this is the current DWORD write pointer (volatile) */
	int MaxDWORDS;	/* the size of the storage */
	unsigned int bvDWORDcntr;	/* counter for  dword index */
	unsigned int bvBitcntr; /* bit counter for each DWORD MODULO CBV_DWORDBITS*/
	
	cbv32native_t *  bveptr;	/*  this is the current extraction DWORD pointer */
	unsigned int extbitcntr;	/* extraction number of bits MODULO  CBV_DWORDBITS*/
		
	
	
public :


	/*
	 * ResetBitVector 
	 * clear everything for writing
	 * */
	void ResetBitVector(void) {
		int n;
		bvptr=bvptrNV;
		bvDWORDcntr=0;
		bvBitcntr=0;  
		
		//clear memory
		n = MaxDWORDS;
		while (n--) {
			*(bvptr++)=0;
		}
		bvptr=bvptrNV; /* reset pointer to start*/
	}



	/*
	 * Cbv32(bv32_t * bv, uint32_t * p, uint32_t numbits)
	 * initialize a bit vector
	 * 
	 * */
	Cbv32(int numbits) {
		int numDWORDS = (numbits/CBV_DWORDBITS)+1;
		bvptrNV=(cbv32native_t * )  malloc(sizeof(cbv32native_t) * numDWORDS);
		MaxDWORDS = numDWORDS;
		ResetBitVector();

		 
	 }
	 
	 ~Cbv32() {
		 free(bvptrNV);
		 bvptrNV=NULL;
	 }
	 
	 #if 1
	 Cbv32 ( Cbv32 * c) {
		/* copy constructor */
		MaxDWORDS = c->MaxDWORDS;
		bvptrNV= new cbv32native_t[MaxDWORDS];
		
		/* do not  ResetBitVector() - no need to clear memory */
		/* copy */
		memcpy(bvptrNV,c->bvptrNV,(bvDWORDcntr+1) * sizeof(cbv32native_t)); /* copy at least one type */
		bvDWORDcntr = c->bvDWORDcntr;
		bvBitcntr = c->bvBitcntr;
		bvptr=bvptrNV;
		 
	 } 
	 #endif

	void ClearBits() {
		/* clear all the memory of the bit vecto to zero */
		memset(bvptrNV,0,MaxDWORDS * sizeof(cbv32native_t));
	
	}

	 
	 uint32_t ExtractingBitPos(void) {
		 /* return the number of bits that have been read so far  from the current extraction pointers*/
		 return ((bveptr-bvptrNV)*CBV_DWORDBITS)+extbitcntr;
	 }
	 
	/*
	 * AddBitVector()
	 * add a bit to the bitvector 
	 * 
	 * */
	void AddBitVector(uint32_t b) { //add 1 bit
		uint32_t temp;  
		
		temp=*(bvptr);	//get current pointed dword
		temp|=(b << bvBitcntr);	//add the bit into the current dword
		*(bvptr)=temp;	//and write back
								
		
		//now increment so we always have a spare
		if (++bvBitcntr >CBV_DWORDBITS-1) {
			bvBitcntr=0;
			bvDWORDcntr++;
			if (bvDWORDcntr >= MaxDWORDS) {
				assert(0); //"AddBitVector ran off the end of the array");
				
			}
			else {
				(bvptr)++; 
				*(bvptr)=0;	//clear next location
			}

		
		}


	}



	 
	 
	/*
	 * void BVSetLength(bv32_t * b, int length)
	 * preset to a certain length to allow random write access
	 * 
	 * */
	void BVSetLength(int length){
		 //figure out where this bit is
		int dw;
		int nbit;

		dw = length / CBV_DWORDBITS;
		nbit = length % CBV_DWORDBITS;	//get remainder.

		if (dw >= MaxDWORDS) {
			assert(0);//"SetBitVectorByBit ran off the end of the array");
					
		}	
		else {
			bvDWORDcntr=dw;
			bvBitcntr=nbit; 		
		}
		
	}

	/*
	 * AddBitVectorBits
	 * add lots of bits to the bit vector
	 * */
	void AddBitVectorBits(uint32_t v, int num){  
		uint32_t temp;
		
		while (num--) {
			temp=*(bvptr);	//get current  
			
			//load bits LSB first.
			temp|=((v&0x1) << bvBitcntr);	//load first bit in
			
			*(bvptr)=temp;	//update memory
			
			v >>=1;	//shift the vector we are adding right- load LSB first
			
			//now increment so we always have a spare
			if (++bvBitcntr >CBV_DWORDBITS-1) {
				bvBitcntr=0;
				bvDWORDcntr++;
				if (bvDWORDcntr >= MaxDWORDS) {
					assert(0);//"AddBitVectorBits ran off the end of the array");
					
				}
				else {
					(bvptr)++; 
					*(bvptr)=0;	//clear next location
				}
			
			} 
		}
	}             


	/*
	 * GetNumBitsInVector()
	 * return the number of bits in the bit vector
	 * 
	 * */
	int GetNumBitsInVector() {
		return  ((uint32_t)bvDWORDcntr)*CBV_DWORDBITS +  (uint32_t) bvBitcntr;
	}       


	/*
	 * SetBitVectorByBit(1,2,3)
	 * modifiy nth bit in a bit vector- 
	 * IE provide random write access to the bit vector. (used for interleaving writer ) 
	 * */
	 
	void SetBitVectorByBit(int bitnum, int value) {
		 //figure out where this bit is
		int dw;
		int nbit;
		uint32_t temp;
		dw = bitnum / CBV_DWORDBITS;
		nbit = bitnum % CBV_DWORDBITS;	//get remainder.

		if (dw >=MaxDWORDS) {
			assert(0);//"SetBitVectorByBit ran off the end of the array");
					
		}
		else {
			//get dword
			temp = *(bvptrNV + dw);
			
			//generate a mask
			temp &= ~(1<<nbit);
			temp |= (value <<nbit);
			*(bvptrNV + dw) = temp;	//writeback the dword
		}
	 }
			   
	/* set nbits up to dword size,  at location bitnum*/
	void SetBitVectorDWORD_at_Bitnum(int bitnum, uint32_t value, int nbits) {
		 //figure out where this bit is
		int dw;
		int nbit;
		uint32_t temp;
		/* shift the byte  in one bit at a time */
		/* put in LSB first , so highest value bit is at highest BV bit*/
		
		for (int i=0;i<nbits-1;i++) {
			dw = bitnum / CBV_DWORDBITS;
			nbit = bitnum % CBV_DWORDBITS;	//get remainder.

			if (dw >=MaxDWORDS) {
				assert(0);//"SetBitVectorByBit ran off the end of the array");
						
			}
			else {
				//get dword
				temp = *(bvptrNV + dw);
				//generate a mask
				temp &= ~(1<<nbit);
				temp |= ((value&0x1) <<nbit);
				*(bvptrNV + dw) = temp;	//writeback the dword
			}
			
			bitnum++; /* go to next bit */
			value>>=1;
		}
	 }


	/*
		now, an EXTraction routines
		



	*/     


	/*
	 * ResetBitVectorExt () extraction
	 * reset to start
	 * return number to read
	 * */

	int  ResetBitVectorExt() {   
		bveptr=bvptrNV;
		extbitcntr=0;   
		return (bvDWORDcntr * CBV_DWORDBITS)+bvBitcntr;

	}               


	/*
	 * GetBitVectorExt()
	 * return next bit.
	 * 
	 * */
	int GetBitVectorExt() {
		//return next bit
		
		int out;
		
		out= (*(bveptr) >> extbitcntr)&0x1;        
		
		//now increment
		if (++extbitcntr > CBV_DWORDBITS-1) {
			extbitcntr=0;
			bveptr++;  
			if ((bveptr - bvptrNV )  >= MaxDWORDS) {
				assert(0);//"GetBitVectorExt ran off the end of the array");
			}
		}     
		return out;
	}   


	/* 
	 * GetBitVectorBitsExt
	 * get numbits in a LSB based vector
	 * */
	uint32_t GetBitVectorBitsExt(int numbits) {
		//return next bits 
		

		uint32_t out=0; 
		int nextbitout;  
		uint32_t temp;  
		uint32_t outbitposcounter=0;
		
		while (numbits--) {//operate on one byte at a time
			temp=*(bveptr);//get current dword
			
			//add this lsb in (we extract in bit order) 
			
			
			nextbitout= (temp >> extbitcntr)&0x1;
			out |= ((uint32_t)nextbitout << (outbitposcounter++)) ;//place this bit left of the previous. 
			
				  
			
			//now increment
			if (++extbitcntr >CBV_DWORDBITS-1) {//dword orientated readout
				extbitcntr=0;
				(bveptr)++;    
				if ((bveptr - bvptrNV )  >= MaxDWORDS) {
					assert(0);//"GetBitVectorExt ran off the end of the array");
				}
		
			}
		}    
		return out;
	}  

	/*
	 * GetBitVectorBitExt
	 * return nth bit in bit vector.
	 * 
	*/   
	int GetBitVectorBitExt(int bitnum) { 
		//figure out location
		int dw;
		int nbit;
		uint32_t temp;
		dw = bitnum / CBV_DWORDBITS;
		nbit = bitnum % CBV_DWORDBITS;	//get remainder.

		if (dw >=MaxDWORDS) {
			assert(0);//"SetBitVectorByBit ran off the end of the array");
					
		}	
		
		//get dword
		temp = *(bvptrNV + dw);
		//get the bit
		return (temp >> nbit) &0x1;

	}

	/*
	 *  int bvCompare(bv32_t * a, bv32_t * b, int nbits) 
	 * compare two bitvectors
	 * 
	 * */
	int bvCompare(Cbv32 * b, int comparebits) {
		//compare dwords until bits only.
		int dws;
		int x;
		unsigned int compare;
		uint32_t * ap, * bp;
		int nbits;
		unsigned int mask;
		Cbv32  * a = this;
		
		dws = comparebits / CBV_DWORDBITS;
		nbits =comparebits % CBV_DWORDBITS;
		
		//reset for reading.
		a->ResetBitVectorExt();
		b->ResetBitVectorExt();
		
		ap = a->bvptrNV;	/* reset the dword pointers to the top */
		bp = b->bvptrNV;
		x=dws;
		compare =0;
		while (x--) {
			compare += (*ap) ^ (*bp);
			ap++;bp++;
		}
		/* done the dwords, now do the bits */
		
		//generate mask of nbits
		x = nbits;
		mask=0;
		while (x--) {
			mask <<=1;
			mask |=0x1;
			
		}
		/* now have mask */
		compare += ((*ap)  ^ (*bp)) & mask;
		return compare;
		
		
		
		
	}



	/*
	 *  int bvDiffCount(bv32_t * a, bv32_t * b, int nbits) 
	 * compare two bitvectors and count number of different bits
	 * 
	 * */
	int bvDiffCount(Cbv32  * b, int comparebits) {
		//compare dwords until bits only.
		int dws;
		int x;
		unsigned int  compare,bitdiffcount;
		uint32_t * ap, * bp;

		int nbits;
		unsigned int mask;
		Cbv32  * a=this;
		dws = comparebits / CBV_DWORDBITS;
		nbits =comparebits % CBV_DWORDBITS;
		
		//reset for reading.
		a->ResetBitVectorExt();
		b->ResetBitVectorExt();
		
		ap = a->bvptrNV;	/* reset the dword pointers to the top */
		bp = b->bvptrNV;
		x=dws;
		compare =0;
		bitdiffcount=0;
		while (x--) {
			compare = (*ap) ^ (*bp);

			while (compare) {
				if (compare & 0x1) bitdiffcount++;
				compare>>=1;
			}
			ap++;bp++;
		}
		/* done the dwords, now do the bits */
		
		//generate mask of nbits
		x = nbits;
		mask=0;
		while (x--) {
			mask <<=1;
			mask |=0x1;
			
		}
		/* now have mask */
		compare = ((*ap)  ^ (*bp)) & mask;
		while (compare) {
			if (compare & 0x1)  bitdiffcount++;
			compare>>=1;
		}
		return bitdiffcount;
		
		
		
		
	}


	/*
	 *  int bvDiffbv(bv32_t * a, bv32_t * b, int nbits, bv32_t diffdv) 
	 * compare two bitvectors and populate another bitvector with the error map
	 * 
	 * */
	int bvDiffbv( Cbv32 * b, int comparebits, Cbv32 * diffbv) {

		//reset for reading.
		Cbv32 * a= this;
		a->ResetBitVectorExt();
		b->ResetBitVectorExt();
		
		while (comparebits--) {
			diffbv->AddBitVector(a->GetBitVectorExt() ^ b->GetBitVectorExt() );
		}
		return 0;
	}

	void bvCopy(Cbv32 *dest) {
		

		cbv32native_t * srcdata, * destdata;
		int ndwords;
		Cbv32 * s=this; /* s = source */
		
		s->ResetBitVectorExt();
		/* figure out where the write pointer currently is so we can copy the offset*/
		
		dest->bvptr = dest->bvptrNV +  (s->bvptr - s->bvptrNV) ;	/* copy writing offset */
		
		/* do not touch the destination non volatile pointer - that is fixed */
		/* do not touch the destination MaxDWORDS- that is fixed */
		dest->bvDWORDcntr = s->bvDWORDcntr;
		dest->bvBitcntr = s->bvBitcntr;
		
		/* copy extraction dword pointer */
		dest->bveptr = dest->bvptrNV + (s->bveptr - s->bvptrNV);
		
		dest->extbitcntr = s->extbitcntr;
		
		/* now copy the data */
		srcdata = s->bvptrNV;
		destdata = dest->bvptrNV;
		ndwords = dest->MaxDWORDS;
		while(ndwords--) {
			*(destdata++) = *(srcdata++);
		}

	}

	



};
#endif				  

