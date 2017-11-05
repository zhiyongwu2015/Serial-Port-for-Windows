

// double jd; 输入参数: 地理坐标的经度，以秒为单位
// double wd; 输入参数: 地理坐标的纬度，以秒为单位
// short DH; 输入参数: 三度带或六度带的带号
//计算时以秒为单位来换算。
void GeoToGauss(double jd, double wd, double* y, double* x)
{
    double t;     // t=tgB
    double L=117;     // 中央经线的经度，以3度带为基准，北京地处115.5-118.5度为39度带中央经线为东经117度
    double l0;    // 经差
    double jd_hd, wd_hd;  // 将jd、wd转换成以弧度为单位
    double et2;    // et2 = (e' ** 2) * (cosB ** 2)
    double N;     // N = C / sqrt(1 + et2)
    double X;     // 克拉索夫斯基椭球中子午弧长
    double m;     // m = cosB * PI/180 * l0
    double tsin, tcos;   // sinB,cosB
    double PI = 3.14159265358979;
    double b_e2 = 0.0067385254147;
    double b_c = 6399698.90178271;
    jd_hd = jd* PI / 180.0;    // 将以秒为单位的经度转换成弧度
    wd_hd = wd * PI / 180.0;    // 将以秒为单位的纬度转换成弧度

    // 如果不设中央经线（缺省参数: -1000），则计算中央经线，
    // 否则，使用传入的中央经线，不再使用带号和带宽参数 
    //L = (DH - 0.5) * DH_width ; // 计算中央经线的经度
/*if (LP == -1000)
    {
        L = (DH - 0.5) * DH_width;      // 计算中央经线的经度
    }
    else
    {
        L = LP;
    }*/

    l0 = jd - L;       // 计算经差
    tsin = sin(wd_hd);        // 计算sinB
    tcos = cos(wd_hd);        // 计算cosB
    // 计算克拉索夫斯基椭球中子午弧长X
    X = 111134.8611* wd - (32005.7799 * tsin + 133.9238 * pow(tsin, 3)
          + 0.6976 * pow(tsin, 5) + 0.0039 * pow(tsin, 7)) * tcos;
    et2 = b_e2 * pow(tcos, 2);      // et2 = (e' ** 2) * (cosB ** 2)
    N = b_c / sqrt(1 + et2);      // N = C / sqrt(1 + et2)
    t = tan(wd_hd);         // t=tgB
    m = PI / 180 * l0 * tcos;       // m = cosB * PI/180 * l0
    *x = X + N * t * (0.5 * pow(m, 2)
               + (5.0 - pow(t, 2) + 9.0 * et2 + 4 * pow(et2, 2)) * pow(m, 4) / 24.0
              + (61.0 - 58.0 * pow(t, 2) + pow(t, 4)) * pow(m, 6) / 720.0);
    *y = N * (m + (1.0 - pow(t, 2) + et2) * pow(m, 3) / 6.0
                    + (5.0 - 18.0 * pow(t, 2) + pow(t, 4) + 14.0 * et2
                       - 58.0 * et2 * pow(t, 2)) * pow(m, 5) / 120.0);
}