#include "ctvm.h"

BoostDoubleVector Gradient2D(BoostDoubleVector U, unsigned long pixel) // pixel = actual rank number -1 (first rank = U(0))
{
	/*
	* Function: Gradient2D (Di*u)
	* ---------------------------
	* Input type: 'BoostDoubleVector (N)' +  'unsigned long'
	* Give the right gradient and the down gradient of the pixel.
	* Output type: 'BoostDoubleVector (2)'
	*/

	unsigned long n = U.size();
	unsigned long l = sqrt(n); // !
	unsigned long rows = 0;
	unsigned long cols = 0;
	double q = 0;

	BoostDoubleVector Du(2); Du = BoostZeroVector(2);
	BoostDoubleMatrix X(l, l); X = BoostZeroMatrix(l, l);

	X = VectorToMatrix(U, l, l);

	if (pixel >= n)
	{
		std::cout << "WARNING: Gradient's rank bigger than specimen size" << std::endl;
		exit(EXIT_FAILURE);
	}

/****************** Find pixel place in the matrix *******************/
	for (unsigned long i = 0; i < l; ++i) 
	{
		if (!pixel)
		{
			rows = i;
			cols = 0;
			break;
		}
		else
		{
			for (unsigned long j = 0; j < l; ++j)
			{
				q = j / pixel;
				if (q)
				{
					rows = i;
					cols = j;
					break;
				}
			}
			pixel = pixel - l;
		}
	}

/******************** Calculate gradient at the rank i ********************/
	if (cols == l - 1 && rows == l - 1)
	{
		Du(0) = X(rows, cols) - X(rows, cols);
		Du(1) = X(rows, cols) - X(rows, cols);
	}
	else if (cols == l - 1)
	{
		Du(0) = X(rows, cols) - X(rows, cols);
		Du(1) = X(rows, cols) - X(rows+1, cols);
	}
	else if (rows == l - 1)
	{
		Du(0) = X(rows, cols) - X(rows, cols+1);
		Du(1) = X(rows, cols) - X(rows, cols);
	}
	else
	{
		Du(0) = X(rows, cols) - X(rows, cols+1);
		Du(1) = X(rows, cols) - X(rows+1, cols);
	}

	return Du;
}

BoostDoubleMatrix Gradient2DMatrix(BoostDoubleVector U)
{
	/*
	* Function: Gradient2DMatrix
	* --------------------------
	* Input type: 'BoostDoubleVector (N)'
	* Give the right and down gradients for all the pixel.
	* Output type: 'BoostDoubleMatrix(N,2)'
	*/
	unsigned long n = U.size();
	BoostDoubleVector Du(2); Du = BoostZeroVector(2);
	BoostDoubleMatrix D (n, 2); D = BoostZeroMatrix (n,2);

	for (unsigned long i = 0; i < n; ++i)
	{
		Du = Gradient2D(U, i);
		for (int j = 0; j < 2; ++j)
		{
			D(i,j) = Du(j);
		}
	}
	return D;
}

BoostDoubleMatrix Unit_Gradient2DMatrix(BoostDoubleVector U, unsigned long pixel)
{
	/*
	* Function: Unit_Gradient2DMatrix
	* -------------------------------
	* Input type: 'BoostDoubleVector (N)' + 'unsigned long'
	* Give a matrix of unit values (+1 or -1 and 0 elsewhere) to compute the 2D gradient at one rank i from a vector of size N.
	* Output type: 'BoostDoubleMatrix (N,2)'
	*/
	
	unsigned long N = U.size();
	unsigned long L = sqrt(N); // !
	unsigned long i = pixel + 1;

	BoostDoubleMatrix Di(2, N); Di = BoostZeroMatrix(2, N);

	if (pixel >= N)
	{
		std::cout << "WARNING PROGRAM STOP: Gradient's rank bigger than specimen size" << std::endl;
		exit(EXIT_FAILURE);
	}
	for (unsigned long l = 1; l < L; ++l)
	{
		if (pixel == N - 1) break; // Right Gradient AND Down Gradient = 0
		else if (N - 1 - L < pixel && pixel < N - 1)
		{
			Di(0, pixel) = 1; Di(0, pixel + 1) = -1; // Right Gradient U(i) - U(i+1)
			break;									 // Down Gradient = 0
		}
		else if (pixel == l*L-1)
		{
			Di(1, pixel) = 1; Di(1, pixel + L) = -1; // Down Gradient
			break;									 // Right Gradient = 0
		}
		else
		{
			Di(0, pixel) = 1; Di(0, pixel + 1) = -1; // Right Gradient
			Di(1, pixel) = 1; Di(1, pixel + L) = -1; // Down Gradient
		}
	}
return Di;
}

double Lagrangian(BoostDoubleMatrix A, BoostDoubleVector U, BoostDoubleVector B, BoostDoubleMatrix W, BoostDoubleMatrix NU, BoostDoubleVector LAMBDA, double beta, double mu)
{
	/*
	* Function: Lagrangian
	* ----------------------------
	* Input type: 'BoostDoubleMatrix (M,N)' + 'BoostDoubleVector (N)' + 'BoostDoubleVector (M)' + 'BoostDoubleMatrix (N,2)' + 'BoostDoubleMatrix (N,2)' + 'BoostDoubleVector (M)'  + 'unsigned long' + 'unsigned long'
	* Calculate the Augmented Lagrangian function developped by Chengbo Li in his thesis "An efficient algorithm for total variation regularization with applications to the single pixel camera and compressive sensing".
	* Output type: 'double'.
	*/
	double L = 0;
	unsigned long n = U.size();
	BoostDoubleVector Wi (2); Wi = BoostZeroVector(2);
	BoostDoubleVector DiU (2); DiU = BoostZeroVector(2);
	BoostDoubleVector NUi (2); NUi = BoostZeroVector(2);

	for (unsigned long i = 0; i < n; ++i)
	{
		DiU = Gradient2D(U, i);
		for (int j = 0; j < 2; ++j)
		{
			Wi(j) = W(i, j);
			NUi(j) = NU(i, j);
		}
		BoostDoubleVector DIFFi = DiU - Wi;
		double norm_wi = norm_2(Wi); // norm_1 for anisotropic TV or norm_2 for isotropic TV
		double square_norm_diffi = norm_2(DIFFi)*norm_2(DIFFi);

		L = L + norm_wi - inner_prod(NUi, DIFFi) + (beta/2) * square_norm_diffi;
	}
	BoostDoubleVector PROD = prod(A, U);
	BoostDoubleVector DIFF = PROD - B;
	double square_norm_diff = norm_2(DIFF)*norm_2(DIFF);

	L = L - inner_prod(LAMBDA, DIFF) + (mu/2)*square_norm_diff;
return L;
}

BoostDoubleVector Shrike(BoostDoubleVector DiUk, BoostDoubleVector NUi, double beta)
{
	/*
	* Function: Shrike
	* ----------------
	* Input type: 'BoostDoubleVector (2)' + 'BoostDoubleVector (2)' + 'double'
	* Give the minimum gradient of the next step W(i,k+1).
	* Output type: 'BoostDoubleVector (2)'
	*/
	BoostDoubleVector W (2); W = BoostZeroVector(2);
	BoostDoubleVector DIFF = DiUk - NUi/beta;

	double norm_diff = norm_2(DIFF);
	double x = norm_diff - 1/beta;

	if (x < 0) x = 0;

	W = x * (DIFF / norm_diff);

return W;
}

BoostDoubleVector Onestep_Direction(BoostDoubleMatrix A, BoostDoubleVector U, BoostDoubleVector B, BoostDoubleMatrix W, BoostDoubleMatrix NU, BoostDoubleVector LAMBDA, double beta, double mu)
{
	/*
	* Function: Onestep_direction
	* ---------------------------
	* Input type: 'BoostDoubleMatrix (M,N)' + 'BoostDoubleVector (N)' + 'BoostDoubleVector (M)' + 'BoostDoubleMatrix (N,2)' + 'BoostDoubleMatrix (N,2)' + 'BoostDoubleVector (M)' + 'unsigned long' + 'unsigned long'
	* Give the direction of the one-step steepest descent gradient that minimize the "u-subproblem"
	* Output type: 'BoostDoubleVector (N)'.
	*/
	unsigned long n = U.size();
	BoostDoubleVector D (n); D = BoostZeroVector(n);
	BoostDoubleVector Wi (2); Wi = BoostZeroVector(2);
	BoostDoubleVector NUi (2); NUi = BoostZeroVector(2);

	for (unsigned long i = 0; i < n; ++i)
	{
		BoostDoubleMatrix Di = Unit_Gradient2DMatrix(U, i);
		BoostDoubleMatrix tDi = trans(Di);
		BoostDoubleVector DiU = Gradient2D(U, i);
		for (int j = 0; j < 2; ++j)
		{
			Wi(j) = W(i, j);
			NUi(j) = NU(i, j);
		}
		BoostDoubleVector DiU_Wi = -DiU - Wi;
		D = D + beta*prod(tDi, DiU_Wi) - prod(tDi, NUi);
	}

	BoostDoubleVector DIFF = prod(A, U) - B;
	BoostDoubleMatrix tA = trans(A);

	D = D + mu*prod(tA, DIFF) - prod(tA, LAMBDA);

return D;
}

double U_Subfunction(BoostDoubleMatrix A, BoostDoubleVector U, BoostDoubleVector B, BoostDoubleMatrix W, BoostDoubleMatrix NU, BoostDoubleVector LAMBDA, double beta, double mu)
{
	/*
	* Function: U_subfunction
	* -----------------------
	* Input type: 'BoostDoubleMatrix (M,N)' + 'BoostDoubleVector (N)' + 'BoostDoubleVector (M)' + 'BoostDoubleMatrix (N,2)' + 'BoostDoubleMatrix (N,2)' + 'BoostDoubleVector (M)' + 'unsigned long' + 'unsigned long'
	* Give the value of the quadratic function Qk(U) one-step steepest descent gradient that minimize the "u-subproblem"
	* Output type: 'double'.
	*/
	double Q = 0;
	unsigned long n = U.size();
	BoostDoubleVector NUi (2); NUi = BoostZeroVector(2);
	BoostDoubleVector Wi (2); Wi = BoostZeroVector(2);

	for (unsigned long i = 0; i < n; ++i)
	{
		BoostDoubleVector DiU = Gradient2D(U, i);
		for (int j = 0; j < n; ++j)
		{
			NUi(j) = NU(i, j);
			Wi(j) = W(i, j);
		}
		BoostDoubleVector DIFFk = DiU - Wi;
		double square_norm_diffk = norm_2(DIFFk)*norm_2(DIFFk);

		Q = Q - inner_prod(NUi, DIFFk) + (beta / 2) * square_norm_diffk;
	}
	BoostDoubleVector PROD = prod(A, U);
	BoostDoubleVector DIFF = PROD - B;
	double square_norm_diff = norm_2(DIFF)*norm_2(DIFF);

	Q = Q - inner_prod(LAMBDA, DIFF) + (mu / 2)*square_norm_diff;
	return Q;
}

BoostDoubleMatrix Alternating_Minimisation(BoostDoubleMatrix A, BoostDoubleVector U, BoostDoubleVector B, BoostDoubleMatrix W, BoostDoubleMatrix NU, BoostDoubleVector LAMBDA, double beta, double mu)
{
	/*
	* Function: Alternating_Minimisation
	* ----------------------------------
	* Input type: 'BoostDoubleMatrix (M,N)' + 'BoostDoubleVector (N)' + 'BoostDoubleVector (M)' + 'BoostDoubleMatrix (N,2)' + 'BoostDobleMatrix (N,2)' + 'BoostDoubleVector (M)' + 'unsigned long' + 'unsigned long'
	* Give the minima W(i,k+1) and U(k+1) of the augmented lagrangian function (W is a matrix of size (N,2) which is collected in AL_MIN(N,0 and 1), U is a vector of size (N) which is collected in AL_MIN(N,2)).
	* Output type: 'BoostDoubleMatrix (N,3)'.
	*/
	double n = U.size();
	double delta = 0.5;
	double rho = 0.5;
	double eta = 0.5;
	double Pk = 1;
	double C = Lagrangian(A, U, B, W, NU, LAMBDA, beta, mu);

	double armijo_tol, Qk, innerstop;
	double tol = 0.01;

	BoostDoubleMatrix AL_MIN (n, 3); AL_MIN = BoostZeroMatrix(n, 3); // AL_MIN(:,0) and AL_MIN(:,1) = W(k+1), AL_MIN(:,2) = U(k+1)

	BoostDoubleVector Uk_1 (n); Uk_1 = BoostZeroVector(n);
	BoostDoubleVector Uk = U;
	do
	{
//*************************** "w sub-problem" ***************************
		for (unsigned long i = 0; i < n; ++i)
		{
			BoostDoubleVector DiUk = Gradient2D(Uk, i);
			BoostDoubleVector NUi(2); NUi = BoostZeroVector(2);
			for (int j = 0; j < 2; ++j)
			{
				NUi(j) = NU(i, j);
				BoostDoubleVector Wi = Shrike(DiUk, NUi, beta);
				W(j, i) = Wi(j);
			}
		}
//*************************** "u sub-problem" ***************************
		BoostDoubleVector Sk = Uk - Uk_1;
		BoostDoubleVector Dk_1 = Onestep_Direction(A, Uk_1, B, W, NU, LAMBDA, beta, mu);
		BoostDoubleVector Dk = Onestep_Direction(A, Uk, B, W, NU, LAMBDA, beta, mu);
		BoostDoubleVector Yk = Dk - Dk_1;

		//******** alpha = onestep_gradient ********
		double alpha = inner_prod(Sk, Yk) / inner_prod(Yk, Yk);
		do 
		{ 
			alpha = rho * alpha;

			BoostDoubleVector Uk_alphaD = Uk - alpha * Dk;
			Qk = U_Subfunction(A, Uk_alphaD, B, W, NU, LAMBDA, beta, mu);
			armijo_tol = C - delta*alpha*inner_prod(Dk, Dk);
		} while (Qk > armijo_tol);

		BoostDoubleVector Uk1 = Uk - alpha * Dk;
		innerstop = norm_2(Uk1 - Uk);

//************************ Implement coefficents ************************
		double Pk1 = eta*Pk + 1;
		double Qk1 = U_Subfunction(A, Uk1, B, W, NU, LAMBDA, beta, mu);
		C = (eta*Pk*C + Qk1)/Pk1;

		Pk = Pk1;
		Uk_1 = Uk;
		Uk = Uk1;
	} while (innerstop > tol);
	
	for (unsigned long pix = 0; pix < n; ++pix)
	{
		AL_MIN(pix, 0) = W(pix, 0);
		AL_MIN(pix, 1) = W(pix, 1);
		AL_MIN(pix, 2) = Uk(pix);
	}
return AL_MIN;
}

//BoostDoubleMatrix tval3_reconstruction(BoostDoubleMatrix Sinogram, BoostDoubleVector TiltAngles) // Why TiltAngles?
BoostDoubleMatrix tval3_reconstruction(BoostDoubleMatrix A, BoostDoubleVector y, unsigned long SideLength)
{
	/*
	* Function: tval3_reconstruction
	* ------------------------------
	* Input type: 'BoostDoubleMatrix (L,A)' + 'BoostDoubleVector (A)'
	* L:size of the specimen, A:number of tilt angles
	* Give a reconstructed image of electron tomography based on the TVAL3 method
	* Output type: 'BoostDoubleMatrix (L,L)'
	*/
	unsigned long l = Sinogram.size1(); // Size of the sample (in pixels)
	unsigned long o = Sinogram.size2(); // Numbers of tilt angles
	unsigned long m = l * o; // Numbers of measurements
	unsigned long n = l * l;
	
	double mu = 3;
	double beta = sqrt(2);
	double coef = 1.05;
	double outerstop;
	double tol = 0.01;

	BoostDoubleMatrix W (n, 2); W = BoostZeroMatrix(n, 2);// W(i,0) = 0 for all i
	BoostDoubleMatrix Wk (n, 2); Wk = BoostZeroMatrix(n, 2);
	BoostDoubleMatrix NU (n, 2); NU = BoostZeroMatrix(n, 2);
	BoostDoubleMatrix A (m, n); A = BoostZeroMatrix(m, n);
	BoostDoubleMatrix MIN (n, 3); MIN = BoostZeroMatrix(n, 3);
	BoostDoubleMatrix X (l, l); X = BoostZeroMatrix(l, l);
	
	BoostDoubleVector U (n); U = BoostZeroVector(n); // U(0) = 0 for all i
	BoostDoubleVector Uk (n); Uk = BoostZeroVector(n); 
	BoostDoubleVector B (m); B = BoostZeroVector(m);
	BoostDoubleVector LAMBDA (m); LAMBDA = BoostZeroVector(m);

	A = CreateRandomMatrix(m, n);
	B = MatrixToVector(Sinogram);
	
	do
	{
		Wk = W;
		Uk = U;
		MIN = Alternating_Minimisation(A, Uk, B, Wk, NU, LAMBDA, beta, mu);
		for (unsigned long i = 0; i < n; ++i)
		{
			W(i, 0) = MIN(i, 0);
			W(i, 1) = MIN(i, 1);
			U(i) = MIN(i, 2);
		}
		BoostDoubleMatrix DiU = Gradient2DMatrix(U);
		NU = NU - beta*(DiU - W); // To verrify
		LAMBDA = LAMBDA - mu*(prod(A, U) - B);

		beta = coef*beta;
		mu = coef*beta;

		outerstop = norm_2(U - Uk);
	} while (outerstop > tol);

	X = VectorToMatrix(U, l, l);
return X;
}
