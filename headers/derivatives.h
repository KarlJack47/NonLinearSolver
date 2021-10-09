#ifndef __DERIVATIVES_H__
#define __DERIVATIVES_H__

#include "general.h"

#define M_GAMMAl 0.5772156649015328606065120900824024L

long double dfmax_xl(long double x, long double y) {
	if (x >= y) return 1.0L;
	else return 0.0L;
}
long double dfmax_yl(long double x, long double y) {
	return dfmax_xl(y, x);
}
long double dfmin_xl(long double x, long double y) {
	if (x <= y) return 1.0L;
	else return 0.0L;
}
long double dfmin_yl(long double x, long double y) {
	return dfmin_xl(y, x);
}

long double dceill(long double x) {
	if (!(sinl(M_PIl * x) == 0.0L)) return 0.0L;
	else return NAN;
}
long double dfloorl(long double x) {
	return dceill(x);
}
long double dfmod_xl(long double x, long double y) {
	if (!(sinl(M_PIl * (x / y)) == 0.0L)) return 1.0L;
	else return NAN;
}
long double dfmod_yl(long double x, long double y) {
	if (!(sinl(M_PIl * (x / y)) == 0.0L)) return -floorl(x / y);
	else return NAN;
}

long double dpowl(long double x, long double n) {
	return n * powl(x, n - 1.0L);
}
long double dsqrtl(long double x) {
	return dpowl(x, 1.0L / 2.0L);
}
long double dcbrtl(long double x) {
	return dpowl(x, 1.0L / 3.0L);
}

long double dfabsl(long double x) {
	return x / fabsl(x);
}
long double dhypot_xl(long double x, long double y) {
	return dpowl(powl(x, 2.0L) + powl(y, 2.0L), 1.0L / 2.0L) * dpowl(x, 2.0L);
}
long double dhypot_yl(long double x, long double y) {
	return dhypot_xl(y, x);
}

long double dsinl(long double x) {
	return cosl(x);
}
long double dcosl(long double x) {
	return -sinl(x);
}
long double dtanl(long double x) {
	return powl((1.0L / cosl(x)), 2.0L);
}

long double dasinl(long double x) {
	return 1.0L / sqrtl(1.0L - powl(x, 2.0L));
}
long double dacosl(long double x) {
	return -dasinl(x);
}
long double datanl(long double x) {
	return 1.0L / (powl(x, 2.0L) + 1.0L);
}

long double dexpl(long double x) {
	return expl(x);
}
long double dlogl(long double x) {
	return 1.0L / x;
}
long double dexp2l(long double x) {
	return exp2l(x);
}
long double dlog2l(long double x) {
	return 1.0L / (x * logl(2.0L));
}
long double dlog10l(long double x) {
	return 1.0L / (x * logl(10.0L));
}

long double dsinhl(long double x) {
	return coshl(x);
}
long double dcoshl(long double x) {
	return sinhl(x);
}
long double dtanhl(long double x) {
	return powl(1.0L / (coshl(x)), 2.0L);
}

long double dasinhl(long double x) {
	return 1.0L / sqrtl(powl(x, 2.0L) + 1.0L);
}
long double dacoshl(long double x) {
	return 1.0L / (sqrtl(x - 1.0L) * sqrtl(x + 1.0L));
}
long double datanhl(long double x) {
	return 1.0L / sqrtl(1.0L - powl(x, 2.0L));
}

// Based on implementation by Jet Wimp, Math. Comp. vol 15 no 74 (1961) pp 174 (see Table 1)
long double digammal(long double x) {
	// force into the interval [1, 3]
	if (x < 0.0L)
		return digammal(1.0L - x) + M_PIl / tanl(M_PIl * (1.0L-x)); // reflection formula
	else if (x < 1.0L)
		return digammal(1.0L + x) - 1.0L / x;
	else if (x == 1.0L)
		return -M_GAMMAl;
	else if (x == 2.0L)
		return 1.0L - M_GAMMAl;
	else if (x == 3.0L)
		return 1.5L - M_GAMMAl;
	else if (x > 3.0L)
		// duplication formula
		return 0.5L * (digammal(x / 2.0L) + digammal((x + 1.0L) / 2.0L)) + M_LN2l;
	else {
		static long double Kncoe[] = {.30459198558715155634315638246624251L,
		.72037977439182833573548891941219706L, -.12454959243861367729528855995001087L,
		.27769457331927827002810119567456810e-1L, -.67762371439822456447373550186163070e-2L,
		.17238755142247705209823876688592170e-2L, -.44817699064252933515310345718960928e-3L,
		.11793660000155572716272710617753373e-3L, -.31253894280980134452125172274246963e-4L,
		.83173997012173283398932708991137488e-5L, -.22191427643780045431149221890172210e-5L,
		.59302266729329346291029599913617915e-6L, -.15863051191470655433559920279603632e-6L,
		.42459203983193603241777510648681429e-7L, -.11369129616951114238848106591780146e-7L,
		.304502217295931698401459168423403510e-8L, -.81568455080753152802915013641723686e-9L,
		.21852324749975455125936715817306383e-9L, -.58546491441689515680751900276454407e-10L,
		.15686348450871204869813586459513648e-10L, -.42029496273143231373796179302482033e-11L,
		.11261435719264907097227520956710754e-11L, -.30174353636860279765375177200637590e-12L,
		.80850955256389526647406571868193768e-13L, -.21663779809421233144009565199997351e-13L,
		.58047634271339391495076374966835526e-14L, -.15553767189204733561108869588173845e-14L,
		.41676108598040807753707828039353330e-15L, -.11167065064221317094734023242188463e-15L};

		register long double Tn_1 = 1.0L; // T_{n-1}(x), started at n=1
		register long double Tn = x - 2.0L; // T_{n}(x), started at n=1
		register long double resul = Kncoe[0] + Kncoe[1]*Tn;

		x -= 2.0L;

		for (int n = 2; n < sizeof(Kncoe)/sizeof(long double); n++) {
			const long double Tn1 = 2.0L * x * Tn - Tn_1; // Chebyshev recursion, Eq. 22.7.4 Abramowitz-Stegun
			resul += Kncoe[n] * Tn_1;
			Tn_1 = Tn;
			Tn = Tn1;
		}

		return resul;
	}
}
long double dtgammal(long double x) {
	return tgammal(x) * digammal(x);
}
long double dlgammal(long double x) {
	return digammal(x);
}

long double djnl(int n, long double x) {
	return (1.0L / 2.0L) * (jnl(n-1, x) - (jnl(n, x) + x * jnl(n+1, x)) / x);
}
long double dj0l(long double x) {
	return djnl(0, x);
}
long double dj1l(long double x) {
	return djnl(1, x);
}

long double dynl(int n, long double x) {
	return (1.0L / 2.0L) * (ynl(n-1, x) - ynl(n+1, x));
}
long double dy0l(long double x) {
	return dynl(0, x);
}
long double dy1l(long double x) {
	return dynl(1, x);
}

long double derfl(long double x) {
	return (2.0L / sqrtl(M_PIl)) * expl(powl(-x, 2.0L));
}
long double derfcl(long double x) {
	return -derfl(x);
}

#endif // __DERIVATIVES_H__
