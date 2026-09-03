function result = p300_erp_baseline(epochs, times, baseline, p300Window)
%P300_ERP_BASELINE Educational P300/ERP summary.
% epochs: trials x samples x channels; times in seconds.

if nargin < 3 || isempty(baseline), baseline = [-0.2 0]; end
if nargin < 4 || isempty(p300Window), p300Window = [0.25 0.50]; end

bmask = times >= baseline(1) & times <= baseline(2);
if ~any(bmask), error('Baseline interval has no samples'); end
base = mean(epochs(:,bmask,:),2);
ep = epochs - base;
erp = squeeze(mean(ep,1));
if isvector(erp), erp = erp(:); end

pmask = times >= p300Window(1) & times <= p300Window(2);
if ~any(pmask), error('P300 window has no samples'); end
p = erp(pmask,:);
t = times(pmask);
amp = mean(p,1);
[~,idx] = max(p,[],1);
latency = t(idx);

result.correctedEpochs = ep;
result.erp = erp;
result.p300MeanAmplitude = amp;
result.p300PeakLatency_s = latency;
end
