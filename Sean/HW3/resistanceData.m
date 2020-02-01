function [Sppm, rho20, rho30] = resistanceData
% resistanceData  Use salinity data from Schlumberger to predict resistance
%                 of the salinity sensor in the ME 121 fish tank

% -- Resistivity data at 20C and 30C.  Sppm is salinity in parts per million
Sppm  = [ 500  600  700  800  1000  1200  1400  1700  2000];  % ppm
rho20 = [11.2  9.5  8.0  7.1   5.6   4.8   4.2   3.4   2.9];  % Ohm-m at 20C
rho30 = [ 9.0  7.5  6.5  5.6   4.6   3.8   3.3   2.8   2.4];  % Ohm-m at 30C

end