function features = hjorth(data)
%HJORTH Compute Hjorth activity, mobility and complexity per channel.

[x, ~] = ads1299eeg.validate_continuous(data);
if any(~isfinite(x(:)))
    error('ads1299eeg:hjorth:NonFinite', 'Hjorth features require finite input data.');
end

dx = diff(x, 1, 1);
ddx = diff(dx, 1, 1);
activity = var(x, 1, 1);
varDx = var(dx, 1, 1);
varDdx = var(ddx, 1, 1);

mobility = zeros(1,size(x,2));
validActivity = activity > 0;
mobility(validActivity) = sqrt(varDx(validActivity) ./ activity(validActivity));

mobilityDx = zeros(1,size(x,2));
validDx = varDx > 0;
mobilityDx(validDx) = sqrt(varDdx(validDx) ./ varDx(validDx));

complexity = zeros(1,size(x,2));
validMobility = mobility > 0;
complexity(validMobility) = mobilityDx(validMobility) ./ mobility(validMobility);

features.activity = activity;
features.mobility = mobility;
features.complexity = complexity;
end
