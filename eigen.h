#ifndef EIGEN_H
#define EIGEN_H


#include <cmath>
#include <iostream>
#include <malloc.h>

using namespace std;

#define SIGN(a,b) ((b)<0 ? -fabs(a) : fabs(a))

void tred2(int n, double **a, double *d, double *e)
/* Adopted From Numerical Recipes in C (2nd Edition) pp. 474-475 :
   Householder reduction of a real, symmetric matrix a[1..n][1..n]. On output,
   a is replaced by the orthogonal matrix Q effecting the transformation.
   d[1..n] returns the diagonal elements of the tridiagonal matrix,
   and e[1..n] the off-diagonal elements, with e[1] = 0. Several
   statements, as noted in comments, can be omitted if only eigenvalues are
   to be found, in which case a contains no useful information on output.
   Otherwise they are to be included. */
{
    int l,k,j,i;
    double scale,hh,h,g,f;

    for (i=n;i>=2;i--) {
        l=i-1;
        h=scale=0.0;
        if (l > 1) {
            for (k=1;k<=l;k++)
                scale += (double)fabs(a[i][k]);
            if (scale == 0.0)
                e[i]=a[i][l];
            else {
                for (k=1;k<=l;k++) {
                    a[i][k] /= scale;
                    h += a[i][k]*a[i][k];
                }
                f=a[i][l];
                g = (double)(f>0 ? -sqrt(h) : sqrt(h));
                e[i]=scale*g;
                h -= f*g;
                a[i][l]=f-g;
                f=0.0;
                for (j=1;j<=l;j++) {
                    // Next statement can be omitted if eigenvectors not wanted
                    a[j][i]=a[i][j]/h;
                    g=0.0;
                    for (k=1;k<=j;k++)
                        g += a[j][k]*a[i][k];
                    for (k=j+1;k<=l;k++)
                        g += a[k][j]*a[i][k];
                    e[j]=g/h;
                    f += e[j]*a[i][j];
                }
                hh=f/(h+h);
                for (j=1;j<=l;j++) {
                    f=a[i][j];
                    e[j]=g=e[j]-hh*f;
                    for (k=1;k<=j;k++)
                        a[j][k] -= (f*e[k]+g*a[i][k]);
                }
            }
        } else
            e[i]=a[i][l];
        d[i]=h;
    }
    // Next statement can be omitted if eigenvectors not wanted
    d[1]=0.0;
    e[1]=0.0;
    // Contents of this loop can be omitted if eigenvectors not
    //  wanted except for statement d[i]=a[i][i];
    for (i=1;i<=n;i++) {
        l=i-1;
        if (d[i]) {
            for (j=1;j<=l;j++) {
                g=0.0;
                for (k=1;k<=l;k++)
                    g += a[i][k]*a[k][j];
                for (k=1;k<=l;k++)
                    a[k][j] -= g*a[k][i];
            }
        }
        d[i]=a[i][i];
        a[i][i]=1.0;
        for (j=1;j<=l;j++) a[j][i]=a[i][j]=0.0;
    }
}


void tqli(int n, double *d,double *e, double **z)
/* Adopted From Numerical Recipes in C (2nd Edition) pp. 480-481 :
   QL algorithm with implicit shifts, to determine the eigenvalues and
   eigenvectors of a real, symmetric, tridiagonal matrix, or of a real,
   symmetric matrix previously reduced by tred2.  On input, d[1..n] contains
   the diagonal elements of the tridiagonal matrix. On output, it returns the
   eigenvalues.  The vector e[1..n] inputs the subdiagonal elements of the
   tridiagonal matrix, with e[1] arbitrary.  On output e is destroyed.  When
   finding only the eigenvalues, several lines may be omitted, as noted in the
   comments.  If the eigenvectors of a tridiagonal matrix are desired, the
   matrix z[1..n][1..n] is input as the identity matrix.  If the eigenvectors
   of a matrix that has been reduced by tred2 are required, then z is input as
   the matrix output by tred2. In either case, the kth column of z returns the
   normalized eigenvector corresponding to d[k]. */

{
    int m,l,iter,i,k;
    double s,r,p,g,f,dd,c,b;

    for (i=2;i<=n;i++) e[i-1]=e[i];
    e[n]=0.0;
    for (l=1;l<=n;l++) {
        iter=0;
        do {
            for (m=l;m<=n-1;m++) {
                dd=(double)(fabs(d[m])+fabs(d[m+1]));
                if (fabs(e[m])+dd == dd) break;
            }
            if (m != l) {
                if (iter++ == 30) {
                    cout << "Too many iterations in TQLI" << endl;
                    return;
                }
                g=(double)((d[l+1]-d[l])/(2.0*e[l]));
                r=(double)(sqrt((g*g)+1.0));
                g=(double)(d[m]-d[l]+e[l]/(g+SIGN(r,g)));
                s=c=1.0;
                p=0.0;
                for (i=m-1;i>=l;i--) {
                    f=s*e[i];
                    b=c*e[i];
                    if (fabs(f) >= fabs(g)) {
                        c=g/f;
                        r=(double)(sqrt((c*c)+1.0));
                        e[i+1]=f*r;
                        c *= (double)(s=(double)1.0/r);
                    } else {
                        s=f/g;
                        r=(double)sqrt((s*s)+1.0);
                        e[i+1]=g*r;
                        s *= (double)(c=(double)1.0/r);
                    }
                    g=d[i+1]-p;
                    r=(double)((d[i]-g)*s+2.0*c*b);
                    p=s*r;
                    d[i+1]=g+p;
                    g=c*r-b;
                    // Next loop can be omitted if eigenvectors not wanted
                    for (k=1;k<=n;k++) {
                        f=z[k][i+1];
                        z[k][i+1]=s*z[k][i]+c*f;
                        z[k][i]=c*z[k][i]-s*f;
                    }
                }
                d[l]=d[l]-p;
                e[l]=g;
                e[m]=0.0;
            }
        } while (m != l);
    }

    double max_val;
    int max_index;
    for(i=1;i<=n-1;i++) {
        max_val = d[i];
        max_index = i;
        for(k=i+1;k<=n;k++)
            if (max_val < d[k]) { max_val = d[k]; max_index = k; }
        if (max_index != i) {
            e[1] = d[i]; d[i] = d[max_index]; d[max_index] = e[1];
            for(l=1;l<=n;l++) {
                e[1] = z[l][i]; z[l][i] = z[l][max_index]; z[l][max_index] = e[1];
            }
        }
    }
}


void eig_sys (int dimension, double **m, double **eig_vec, double *eig_val)
{
    static double **a = NULL;
    static double *d = NULL, *e = NULL;  // data structure for tred2 and tqli - don't ask
    static int i,j;

    if (a == NULL) {
        a = new double *[dimension+1];
        for(i=0;i<dimension+1;i++) a[i] = new double[dimension+1];
    }
    if (d == NULL) d = new double [dimension+1];
    if (e == NULL) e = new double [dimension+1];

    for(i=0;i<dimension;i++)
        for(j=0;j<dimension;j++) a[i+1][j+1] = m[i][j];

    tred2(dimension,a,d,e);
    tqli(dimension,d,e,a);

    for (i=0;i<dimension;i++) {
        eig_val[i] = d[i+1];
        for (j=0;j<dimension;j++)
            eig_vec[j][i] = a[i+1][j+1];   // order should be swapped
    }
}


#endif // EIGEN_H
