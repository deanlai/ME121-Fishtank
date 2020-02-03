function salinity_calibration
% Salinity_calibration 
% -- Use data recorded from our salinity sensor to generate a model to 
% -- measure salinity as a fucntion of resistance measured from our sensor

% Import data from spreadsheet:
% 4 columns of analog inpout data of measured resistance for solutions of
% known salinity (0.0wt%, 0.05wt%, 0.10wt%, 0.15wt%)
% voltage divider circuit uses a 10k ohm fixed resistor
% Firs 4 rows of sheet are ignored
raw_data = readtable('calibrationdata.xlsx', 'range', '4:149');

% Convert to matrix
raw_data = raw_data{:,:};

% Create 4 data points for curve fitting by averaging columns of raw_data
means = [mean(raw_data(:,1)),...
    mean(raw_data(:,2)),...
    mean(raw_data(:,3)),...
    mean(raw_data(:,4))];

% Round means values to nearest integer
means = round(means);

% Create salinity matrix in wt%
salinity = [0.000001, 0.05, 0.10, 0.15];

% Generate curve fit constants and R^2 from calibration data using powfit
[constants, r_squared] = powfit(salinity, means);

% Generate numnber space for values of salinity
s_fit = linspace(min(salinity), max(salinity));
analog_fit = constants(1)*s_fit.^constants(2);

% Plot Analog output vs. salinity
plot(salinity, means, 'ro', s_fit, analog_fit, 'k--')
xlabel('Salinity (%wt)')
ylabel('Analog output')
title('Analog Output vs. Salinity')

figure
% Plot Salinity vs. analog output
plot(means, salinity, 'ro', analog_fit, s_fit, 'k--')
ylabel('Salinity (%wt)')
xlabel('Analog output')
title('Analog Output vs. Salinity')



