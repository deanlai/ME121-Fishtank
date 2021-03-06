function salinity_calibration_v2
% Salinity_calibration 
% -- Use data recorded from our salinity sensor to generate a model to 
% -- measure salinity as a fucntion of resistance measured from our sensor
% -- v2 uses polyfit to produce a piecewise linear function



% Import data from spreadsheet:
% 4 columns of analog inpout data of measured resistance for solutions of
% known salinity (0.0wt%, 0.05wt%, 0.10wt%, 0.15wt%)
% voltage divider circuit uses a 10k ohm fixed resistor
% Firs 4 rows of sheet are ignored
raw_data = readtable('calibrationdata_v2.xlsx', 'range', '4:120');

% Convert to matrix
raw_data = raw_data{:,:};

% Create 4 data points for curve fitting by averaging columns of raw_data
means = [mean(raw_data(:,1)),...
    mean(raw_data(:,2)),...
    mean(raw_data(:,3)),...
    mean(raw_data(:,4))];

% Round means values to nearest integer
analog = round(means);

% Create salinity matrix in wt%
salinity = [0.00000001, 0.05, 0.10, 0.15];

% Evaluate first degree polynomial fits
% c_fwd --> constants for forward fit, high salinity
c_fwd = polyfit(salinity(2:end), analog(2:end), 1);

% c_rev --> constants for reverse fit, hight salinity
c_rev = polyfit(analog(2:end), salinity(2:end), 1);

% Evaluate polynomial fits at 0.05wt% salinity for fitting the low salinity
% curve. trans_fwd/rev --> forward/reverse transition point
% function has form y = c_1*x + c_2
trans_fwd = c_fwd(1)*salinity(2) + c_fwd(2);
trans_rev = c_rev(1)*analog(2) + c_rev(2);

% Create vectors to generate plots and fits
% for forward plot, x: salinity, y: analog
% for reverse plot, x: analog,   y: salinity
% high is upper curve after 0.5% salinity, low below
x_fwd_high = salinity(2:end);
y_fwd_high = c_fwd(1).*x_fwd_high + c_fwd(2); % Eval y = c(1)x + c(2)
x_fwd_low = salinity(1:2);
y_fwd_low = [analog(1), y_fwd_high(1)];

% Evaluate first degree polynomial fits for low portion of curve
% c_fwd_low --> constants for forward fit, low salinity
c_fwd_low = polyfit(x_fwd_low, y_fwd_low, 1);

% c_rev_low --> constants for reverse fit, low salinity
c_rev_low = polyfit(y_fwd_low, x_fwd_low, 1);

% Print constants to console for use in Arduino control code
fprintf('constants: forward, high\n');
fprintf('%14.7e\n', c_fwd);
fprintf('constants: forward, low\n');
fprintf('%14.7e\n', c_fwd_low);
fprintf('constants: reverse, high\n');
fprintf('%14.7e\n', c_rev);
fprintf('constants: reverse, low\n');
fprintf('%14.7e\n', c_rev_low);
fprintf('breakpoints for function\n');
fprintf(' %d\n', analog(1), y_fwd_high);

% Plot forward calibration curve
% Analog output = f(Salinity)
plot(x_fwd_high, y_fwd_high, 'k-',...
     x_fwd_low, y_fwd_low, 'k-',...
     salinity, analog, 'ro')
xlabel('Salinity (wt%)')
ylabel('Analog Input')
title('Analog Input vs. Salinity')

figure % New figure window
% Plot reverse calibration curve
% Salinity = f(Analog output)
plot(y_fwd_high, x_fwd_high, 'k-',...
     y_fwd_low, x_fwd_low, 'k-',...
     analog, salinity, 'bo');
ylabel('Salinity (wt%)')
xlabel('Analog Input')
title('Salinity vs. Analog Input')

