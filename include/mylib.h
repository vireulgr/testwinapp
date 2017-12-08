#ifndef _MYLIB_H_
#define _MYLIB_H_
// #include <stdlib.h>

void matrixSquare( float * mat, size_t size, float ** res );
void TEST_matrixSquare( );

template< typename T >
void bubbleSort( T * arr, size_t size ) {
//void bubbleSort( float * arr, size_t size ) {
    for( int j=0; j<size; ++j ) {
        for( int i=1; i<size; ++i ) {
            if( arr[i] < arr[i-1] ) {
//                float temp = arr[i];
                T temp = arr[i];
                arr[i] = arr[i-1];
                arr[i-1] = temp;
            }
        }
    }
}
void TEST_bubbleSort( );

template< typename T >
void alternatingHarmonicSeries( int num, T & res ) {
    bool neg = false;
    for( int i=1; i<num; ++i ) {
        res += (neg ? -1.0/i : 1.0/i);
        neg = !neg;
    }
}

void TEST_alternatingHarmoincSeries( );

template< typename T >
void logMcLoren( T arg, size_t prec, T & res ) {
    T member;
    T eks = arg;
    bool neg = false;
    for( int i=1; i<prec; ++i ) {
        member = eks/i;
        if( member == 0.0 ) break;
        //printf( "member: %12g res: %12g eks: %12g neg %d\n", member, res, eks, neg ? 1 : 0 );
        res += (neg ? -member : member);
        eks *= eks;
        neg = !neg;
    }
}

void TEST_logMcLoren( ); 

template< typename T >
void logOverArray( T * arr, size_t size ) {
    for( int i=0; i<size; ++i ) {
        T res = 0.0;
        logMcLoren( arr[i], 20, res );
        arr[i] = res;
    }
}

void TEST_logOverArray();


#endif
