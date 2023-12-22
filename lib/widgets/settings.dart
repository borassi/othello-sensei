import 'package:cupertino_setting_control/cupertino_setting_control.dart';
import 'package:flutter/material.dart';

class SettingsPage extends StatelessWidget {
  const SettingsPage({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        title: 'Settings',
        home: Scaffold(
          appBar: AppBar(title: const Text("Settings"),),
          body: Column(
          children: [
            SettingRow(
              rowData: SettingsYesNoConfig(
                  title: 'Approximate',
                  initialValue: false,
              ),
              config: const SettingsRowConfiguration(
                  showAsTextField: true,
                  showTitleLeft: false,
              showAsSingleSetting: false),
              onSettingDataRowChange: (bool newValue) {print(newValue);},
            )
          ]
        )
      )
    );
  }
}