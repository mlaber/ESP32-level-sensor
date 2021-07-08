  #ifndef PROMETHEUS_METRIC_H
  #define PROMETHEUS_METRIC_H

  #include "Arduino.h"

  enum MetricType : short {
    untyped = 0, counter, gauge, histogram, summary
  };

  class Metric {
    private:
      String name;
      String helpText;
      MetricType type;
      float value;
      int precision;
      String prefix;      // Will be put before prometheus metric name 
    public:
      String getString();
      Metric(MetricType type, String name, String helpText, int precision, String prefix);
      void setValue(float val) {
        this->value = val;
      }
  };

  #endif
