
% ***************** RUN ON INEKS WITH***********
% matlab -nojvm -nosplash -r "the3" > the3.txt ; reset; cat the3.txt;rm -f the3.txt
% The answers will be in the terminal after 30-40 seconds
%**********************************************

%The size of the Monte Carlo study obeying the given specifications can be found with the formula on pg. 116 of the textbook.
% N >= 0.25*((z_(alpha/2))/epsilon)^2, alpha = 1- 0.95 = 0.05, epsilon =0.005 given
% N >= 0.25*((z_0.025)/0.005)^2, z_0.025 = PHI^-1(1-0.025) = 1.96
% N >= 0.25*(1.96/0.005)^2
% N >= 38416, N = 38416
% Our estimator for X has stdweight/sqrt(N) standard deviation. (6.3415/sqrt(38416)) = 0.032
% Therefore the accuracy is likely to be high


Coefs=[0.4 -0.6 0 0.3 ; -1.2 1.8 0 0.1; 1.2 -1.8 0 0.7];
breaks=[0, 1, 2, 3];
P1 = mkpp(breaks, Coefs);

lambda = 12;
N = 38416;
a=0;b=0;c=0;
myEST = zeros(1, N);
weightlist = zeros(1,N);
while N > 0
	p = poissrnd(lambda);
	weights = zeros(1, p);
	while p > 0
		r = rand;
		if (0<=r && r<=0.2)
			a = 0;b = 1;c = 0.3;	
		elseif (0.2 < r&& r <= 0.6)
			a = 1;b = 2;c = 0.7;
		elseif (0.6 < r && r <= 1)
			a = 2;b = 3;c = 0.7;
		end;
			U = rand;
			V = rand;
			X = a+(b-a)*U;
			Y = c*V;
		while (Y > ppval(P1,X))
			U = rand;
			V = rand;
			X = a+(b-a)*U;
			Y = c*V;
		end;
		weights(p) = X;
		p = p - 1;
	end;
    weightlist(N) = sum(weights);
	if(sum(weights) > 25)
		myEST(N) = 1;
	end;
	N = N-1;
end;


prob = mean(myEST)
meanwweight = mean(weightlist)
stdweight = std(weightlist)
quit
