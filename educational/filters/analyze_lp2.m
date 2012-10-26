load lp2ay.csv

N = 512;
tflp2 = fft(lp2ay(:,1), N);

dRng = 1:N/2;
figure(1)
subplot(2,1,1)
f = (0:1/N:0.5-1/N);
sRng = 1:length(f);
plot(f, abs(tflp2(sRng))/32768, f, arg(tflp2(sRng)));
axis tight; grid on;
legend("magnitude", "phase");
xlabel("normalized frequency");
title("lp2 IIR filter");

subplot(2,1,2)
plot(lp2ay);
legend("impulse response");
grid on;
axis tight;
xlabel("sample number");

