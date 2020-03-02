function plot_heater_test(fname)
% plot_heater_test  Plot T(t) to find heater constant K, which is the
%                   slope of the T(t) curve when the heater is on
%
% Synopsis:  plot_heater_test
%            plot_heater_test(fname)
%
% Input:  fname = (string) name of the input data file.
%                 default: heater_test1.txt
%
% Output:  Plots of Temperature versus time and numerical values of
%          dT/dt, which is the slope of the line fit to segments of
%          the T(t) data.
%
% Notes on Usage:
%
%  The code requires a plain text file with three columns separated by
%  tabs or spaces. The three columns are time, heater status, and
%  temperature. The heater status is a number, either 0 or 1. A value of
%  0 in the second column indicates that the heater was off. A value of 1
%  indicates that the heater was on.
%
%  After drawing the T(t) data in a new figure window, the code will pause
%  for the user to click twice on the curve.  Use the mouse input to click
%  on the starting point and stopping point for the region over which K
%  is to be measured.  After the data is selected, the code uses a least
%  squares fit to the subset of data.
%
%  The code will stop a second time for you to select two more points to
%  indicate a region to analyze.  For the second slection, we recommend
%  the range of data after the heater input has stopped and the water
%  has reached a plateau of temperature.  Depending on the conditions,
%  the water temperature may actually decrease.
%
%  Here is a summary of how the code works:
%
%  1. Launch the code by typing plot_heater_test at the command prompt
%  2. Click once at the start of the ramp in T versus t. This defines
%     the start of the first line fit segment.
%  3. Click again at the end of the ramp. This defines the end of the
%     first line fit segment.
%  4. Click once at a point where the steady state behavior appears to
%     have asserted itself again after the heat input ramp has stopped.
%  5. Click one more time a the end of the steady state behavior

% -- Provide a default input file name
if nargin<1, fname = 'heater_test1.txt';  end

% -- Load data into working variables
D = load(fname);
t = D(:,1)/1000;          %  convert milliseconds to seconds
heater_status = D(:,2);
T = D(:,3);

% -- Plot temperature versus time data
figure('Name',sprintf('Original data from %s',fname));
plot(t,T,'.','MarkerSize',12);  ylabel('T (C)');  xlabel('Time (s)');

% -- Label the interval when the valve was open. First, find all indices
%    when the heater was on, then find the first and last of those indices
ion = find( heater_status==1 );
istart = min(ion);
istop = max(ion);

% -- Draw vertical dashed red lines where the heater is started and stopped
minT = min(T);  maxT = max(T);
hold('on');
plot( t(istart)*[1 1], [minT maxT], 'r--');
plot( t(istop)*[1 1], [minT maxT], 'r--');

% -- User indicates starting and stopping points during the ramp in T(t)
%    when the heater is turned on.  Only x-coordinates of the mouse click
%    are important
msgbox('Use the mouse to click along the time axis to select a range of data (y-value of click does not matter',...
           'Select range','modal');
[x,~] = ginput(2);
istart_heat = min( find(t>x(1), 1, 'first') );
istop_heat  = max( find(t<x(2), 1, 'last') );
tramp = t(istart_heat:istop_heat);    % vector of times during the ramp
Tramp = T(istart_heat:istop_heat);    % vector of temperatures during the ramp
c_heat = polyfit(tramp,Tramp,1);
fprintf('\ndT/dt = %8.4f (deg C)/s  = %8.3f (deg C)/min\n',c_heat(1),60*c_heat(1));

% -- User indicates starting and stopping points of plateau in T(t) after
%    the ramp in T(t) when the heater is turned on.  Only x-coordinates of
%    the mouse click are important
msgbox('Use the mouse again to select two more points','Select range','modal');

[x,~] = ginput(2);
istart = min( find(t>x(1), 1, 'first') );
istop  = max( find(t<x(2), 1, 'last') );
tflat = t(istart:istop);    %  vector of times during the plateau
Tflat = T(istart:istop);    %  vector of temperatures during the plateau
c = polyfit(tflat,Tflat,1);
fprintf('\ndT/dt = %8.4f (deg C)/s  = %8.3f (deg C)/min\n',c(1),60*c(1));

% -- Compute the standard deviation of temperature during the plateau
%    Substract the trend, which is the least squares line fit to the
%    points in the plateau
stdFlat = std(Tflat - polyval(c,tflat));
fprintf('One standard deviation during plateau = %8.3f\n',stdFlat);

% -- Plot temperature versus time data
figure('Name','Temperature slope plot');
grey = [0.7 0.7 0.7];
plot(t,T,'.','MarkerEdgeColor',grey,'MarkerFaceColor',grey,'MarkerSize',16);
ylabel('T (C)');
xlabel('Time (s)');
tfit = [min(tramp) max(tramp)];
Tfit = polyval(c_heat,tfit);
hold('on');
plot(tfit, Tfit, 'r--', 'LineWidth', 2 );
text( t(istart_heat) + 0.3*(t(istop_heat)-t(istart_heat)), ...
      T(istart_heat) - 0.1*(T(istop_heat)-T(istart_heat)),  ...
      sprintf('slope = %7.3f (deg C)/min',60*c_heat(1)))
hold('off');

% -- Add line fit to data during plateau after heating
tfit = [min(tflat) max(tflat)];
Tfit = polyval(c,tfit);
hold('on');
plot(tfit, Tfit, 'r--', 'LineWidth', 2 );
plot(tfit, Tfit+3*stdFlat, 'r--', 'LineWidth', 1.5);
plot(tfit, Tfit-3*stdFlat, 'r--', 'LineWidth', 1.5);
text( t(istart), ...
      T(istart) - 7*stdFlat,  ...
      sprintf('slope = %7.3f (deg C)/min',60*c(1)))
hold('off');

% -- Histogram of data in the plateau:  substract trend before computing
%    the histogram
figure('Name','Histogram for second group of data selected')
histogram(Tflat - polyval(c,tflat), 25)
xlabel('T relative to the trend');
ylabel('Number of samples')

end