function [c,R2] = powfit(x,y)
% expfit     Least squares fit of data to y = c1*x^c2
%
% Synopsis:  c = powfit(x,y)
%
% Input:     x,y = vectors of independent and dependent variable values
%
% Output:    c = vector of coefficients: y = c(1)*x^c(2)

logx = log(x(:));   %  transform x
logy = log(y(:));   %  transform y

ct = polyfit(logx,logy,1);  %  Line fit to transformed data
c = [exp(ct(2));  ct(1)];   %  Extract parameters from transformation

if nargout>1   %  Compute R^2 in the transformed space
  R2 = 1 - sum((logy - polyval(ct,logx)).^2)/sum((logy - mean(logy)).^2);
end

end
