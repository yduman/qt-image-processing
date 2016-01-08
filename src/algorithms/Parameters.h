#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QObject>

class Parameters : public QObject
{
Q_OBJECT

public:
   Parameters();

    bool   param1;
    int    param2;
    double param3;

    int lucyIterations;
    int kernelSize;
    int sigma_d;
    int sigma_r;
    int tau;

public slots:
    void setParam1(bool value) { param1 = value; }
    void setParam2(const QString &value) { param2 = value.toInt(); }
    void setParam3(const QString &value) { param3 = value.toDouble(); }
    void setLucyIterations(const QString &value) { lucyIterations = value.toDouble(); }
    void setKernelSize(const QString &value) { kernelSize = value.toInt(); }
    void setSigma_D(const QString &value) { sigma_d = value.toInt(); }
    void setSigma_R(const QString &value) { sigma_r = value.toInt(); }
    void setTau(const QString &value) { tau = value.toInt(); }
};

#endif
