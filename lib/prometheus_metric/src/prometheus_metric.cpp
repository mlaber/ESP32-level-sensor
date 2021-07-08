// Prometheus expoter reference: https://prometheus.io/docs/instrumenting/writing_exporters/

#include "prometheus_metric.h"
#include "Arduino.h"


String Metric::getString() {
  String result = "";
  if (isnan(this->value)) {
    return result;
  }
  result += String("# HELP ") + this->name + String(" ") + this->helpText + String("\n");
  switch (this->type) {
    case counter:
      result += String("# TYPE ") + this->name + String(" counter\n");
      break;
    case gauge:
      result += String("# TYPE ") + this->name + String(" gauge\n");
      break;
    case histogram:
      result += String("# TYPE ") + this->name + String(" histogram\n");
      break;
    case summary:
      result += String("# TYPE ") + this->name + String(" summary\n");
      break;
  }

  result += this->name + String(" ") + String(this->value, this->precision) + String("\n");
  return result;
}

Metric::Metric(MetricType type, String name, String helpText, int precision, String prefix) {
  this->type = type;
  //this->name = name;
  this->name = prefix + "_" + name; // Put Prefix e.g. hostname before prometheus name
  this->helpText = helpText;
  this->precision = precision;
}
