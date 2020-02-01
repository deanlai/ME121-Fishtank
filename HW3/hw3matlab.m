function salinity_model
% salinity_model  Use salinity data from Schlumberger to predict resistance
%                 of the salinity sensor in the ME 121 fish tank

% -- Resistivity data at 20C and 30C.  Sppm is salinity in parts per million
Sppm  = [ 500  600  700  800  1000  1200  1400  1700  2000];  % ppm
rho20 = [11.2  9.5  8.0  7.1   5.6   4.8   4.2   3.4   2.9];  % Ohm-m at 20C
rho30 = [ 9.0  7.5  6.5  5.6   4.6   3.8   3.3   2.8   2.4];  % Ohm-m at 30C

% Convert salinity data from ppm to wt_percent
wt_percent = Sppm./1000;

plot(wt_percent,rho20,'o',wt_percent,rho30,'o');
xlabel('Salinity (wt%)');
ylabel('Resistivity (Ohm-m)');
title('Fig. 1: Resistivity as a function of salinity');

% Convert resisitivity to resistance
L = 0.020;      % Length between probes on the sensor
d = 0.015;      % Interior diamter of the sensor
r = d/2;        % Converts dimater to radius
A = pi*r^2;     % Calculates cross-sectional area of the conductor
R20 = (rho20*L)/A;
R30 = (rho30*L)/A;

function [c,R2] = powfit(wt_percent,R20)
% expfit     Least squares fit of data to y = c1*x^c2
%
% Synopsis:  c = powfit(x,y)
%
% Input:     x,y = vectors of independent and dependent variable values
%
% Output:    c = vector of coefficients: y = c(1)*x^c(2)

logwt_percent = log(wt_percent(:));   %  transform x
logR20 = log(R20(:));   %  transform y

ct = polyfit(logwt_percent,logR20,1);  %  Line fit to transformed data
c = [exp(ct(2));  ct(1)];   %  Extract parameters from transformation

if nargout>1   %  Compute R^2 in the transformed space
  R2 = 1 - sum((logy - polyval(ct,logwt_percent)).^2)/sum((logR20 - mean(logR20)).^2);
end
end

% -- Curve fit resistance as function of salinity for rho20:  rho = c(1)*Sppm^c(2)
c = powfit(wt_percent, R20);                        %  powfit makes the curve fit
Sfit = linspace(min(wt_percent), max(wt_percent));  %  Generate a vector of 100 values
rho20fit = c(1)*Sfit.^c(2);                         %  Evaluate curve fit at each Sfit(i)

% -- Curve fit resistance as function of salinity for rho30:  rho = c(1)*Sppm^c(2)
c = powfit(wt_percent, R30);                        %  powfit makes the curve fit
Sfit = linspace(min(wt_percent), max(wt_percent));  %  Generate a vector of 100 values
rho30fit = c(1)*Sfit.^c(2);                         %  Evaluate curve fit at each Sfit(i)

% -- Plot on linear axes
figure
plot(wt_percent,R20,'o',Sfit,rho20fit,'r:')  % Origial data is 'o'; fit is red dotted
hold on;
plot(wt_percent,R30,'o',Sfit,rho30fit,'g:')  % Origial data is 'o'; fit is green dotted
hold off;
xlabel('Salinity (wt%)')
ylabel('Resistivity (ohm-m)')
title('Fig. 2: Linear fit of data')

% -- Make loglog plot of the same data
figure   %  open up new plot window
loglog(wt_percent,R20,'o',Sfit,rho20fit,'r:')  % Origial data is 'o'; fit is red dotted
hold on;
loglog(wt_percent,R30,'o',Sfit,rho30fit,'g:');  % Original data is 'o'; fit is green dotted
hold off;
xlabel('Salinity (wt%)')
ylabel('Resistivity (ohm-m)')
title('Fig. 3: Log fit of data')

% Computes voltage divider output
Vs = 12;    % Voltage input
Rf = 10000;    % Value of 10 kOhm resistor

v_out20 = Vs*(Rf./(Rf + rho20fit));
v_out30 = Vs*(Rf./(Rf + rho30fit));

% Plots voltage divider output versus salinity
figure
plot(Sfit,v_out20,'r:');
hold on;
plot(Sfit,v_out30,'g:'); 
hold off;
xlabel('Salinity (wt%)');
ylabel('Analog output (V)');
title('Fig. 4: Analog output as a function of salinity');


end
