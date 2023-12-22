
import 'dart:math';

String prettyPrintDouble(double d) {
//  std::locale::global(std::locale("en_US.UTF8"));
  if (d == double.infinity) {
    return "+Inf";
  } else if (d == -double.infinity) {
    return "-Inf";
  } else if (d.isNaN) {
    return "NaN";
  } else if (d.abs() < 1.0E-20) {
    return "0";
  }
  if (d < 0) {
    return "-${prettyPrintDouble(-d)}";
  }
  const List<String> suffixes = [
      "p", "n", "μ", "m", "", "k", "M", "G",
      "T", "P", "E", "Z", "Y"];
  int orderOfMagnitude = (log(d) / ln10 / 3).floor();
  // Between 1 and 999.999999
  double rescaledD = d / pow(10, 3 * orderOfMagnitude);
  if (rescaledD > 999.5) {
    orderOfMagnitude++;
    rescaledD = 1;
  }
  int suffixesPosition = orderOfMagnitude + 4;
  if (suffixesPosition >= suffixes.length || suffixesPosition < 0) {
    return d.toStringAsPrecision(1);
  }
  String suffix = suffixes[suffixesPosition];

  if (rescaledD < 9.95) {
    return "${rescaledD.toStringAsFixed(1)}$suffix";
  }
  return "${rescaledD.toStringAsFixed(0)}$suffix";
}