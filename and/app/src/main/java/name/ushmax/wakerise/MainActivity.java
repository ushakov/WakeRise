package name.ushmax.wakerise;

import android.app.TimePickerDialog;
import android.bluetooth.BluetoothDevice;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.harrysoft.androidbluetoothserial.BluetoothManager;
import com.harrysoft.androidbluetoothserial.BluetoothSerialDevice;
import com.harrysoft.androidbluetoothserial.SimpleBluetoothDeviceInterface;

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.List;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.schedulers.Schedulers;

public class MainActivity extends AppCompatActivity {
    Logger log = Logger.getLogger(MainActivity.class.getName());

    private Switch onSwitch;
    private TextView timeDisplay;

    private int hours = 12;
    private int minutes = 0;

    private BluetoothManager bluetoothManager = null;
    private SimpleBluetoothDeviceInterface bluetoothDevice = null;

    private boolean tryingToConnect;
    private int backoffSeconds;
    private String targetMAC;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        onSwitch = findViewById(R.id.on_switch);
        onSwitch.setOnClickListener(v -> {
            int id = onSwitch.isChecked() ? 1 : 0;
            bluetoothDevice.sendMessage("swtc" + id + "\n");
            sendCurrentTime();
        });

        timeDisplay = findViewById(R.id.time_display);
        timeDisplay.setOnClickListener(v -> {
            TimePickerDialog timePickerDialog = new TimePickerDialog(
                    this,
                    (view, hourOfDay, minute) -> {
                        bluetoothDevice.sendMessage(String.format("sttm%02d%02d\n", hourOfDay, minute));
                        sendCurrentTime();
                    },
                    hours, minutes, true);
            timePickerDialog.show();
        });
    }

    private void sendCurrentTime() {
        Calendar c = GregorianCalendar.getInstance();
        int hr = c.get(Calendar.HOUR_OF_DAY);
        int mn = c.get(Calendar.MINUTE);
        int sec = c.get(Calendar.SECOND);
        log.info("Current time: "+ hr + ":" + mn + ":" + sec);
        bluetoothDevice.sendMessage(String.format("tmnw%02d%02d%02d\n", hr, mn, sec));
    }

    @Override
    protected void onStart() {
        super.onStart();
        if (!initBluetooth()) {
            finish();
        }
    }

    private boolean initBluetooth() {
        bluetoothManager = BluetoothManager.getInstance();
        if (bluetoothManager == null) {
            // Bluetooth unavailable on this device :( tell the user
            Toast.makeText(this, "Bluetooth not available.", Toast.LENGTH_LONG).show(); // Replace context with your context instance.
            return false;
        }
        List<BluetoothDevice> pairedDevices = bluetoothManager.getPairedDevicesList();
        BluetoothDevice selectedDevice = null;
        for (BluetoothDevice device : pairedDevices) {
            Log.d("My Bluetooth App", "Device name: " + device.getName());
            Log.d("My Bluetooth App", "Device MAC Address: " + device.getAddress());
            if (device.getName().equals("WakeRise")) {
                selectedDevice = device;
            }
        }

        if (selectedDevice == null) {
            Toast.makeText(this, "Please pair with WakeRise lamp and restart app.", Toast.LENGTH_LONG).show();
            return false;
        }

        log.info("Connecting to " + selectedDevice.getAddress());

        tryingToConnect = true;
        backoffSeconds = 0;
        targetMAC = selectedDevice.getAddress();

        startConnecting();
        return true;
    }

    private void startConnecting() {
        if (!tryingToConnect) {
            return;
        }
        onSwitch.setEnabled(false);
        timeDisplay.setEnabled(false);
        if (backoffSeconds > 0) {
            try {
                TimeUnit.SECONDS.sleep(backoffSeconds);
            } catch (InterruptedException e) {
                // do nothing
            }
        }
        bluetoothManager.openSerialDevice(targetMAC)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(this::onConnected, this::onConnectError);
    }

    private void onConnected(BluetoothSerialDevice connectedDevice) {
        tryingToConnect = false;
        backoffSeconds = 0;
        bluetoothDevice = connectedDevice.toSimpleDeviceInterface();
        bluetoothDevice.setMessageReceivedListener(this::onMessageReceived);
        bluetoothDevice.setErrorListener(this::onListenError);
        bluetoothDevice.sendMessage("stat\n");
        onSwitch.setEnabled(true);
        timeDisplay.setEnabled(true);
        log.info("Connected! " + connectedDevice.getMac());
    }

    private void onListenError(Throwable throwable) {
        if (bluetoothDevice != null) {
            bluetoothManager.closeDevice(bluetoothDevice);
        }
        tryingToConnect = true;
        backoffSeconds = 0;
        log.info("Listen error: " + throwable);
        Toast.makeText(this, "Error talking to WakeRise, trying to reconnect.", Toast.LENGTH_LONG).show();
        startConnecting();
    }

    private void onConnectError(Throwable error) {
        backoffSeconds += 2;
        log.info("Error connecting: " + error);
        Toast.makeText(this, "Error connecting to WakeRise, trying again.", Toast.LENGTH_LONG).show();
        startConnecting();
    }

    private void onMessageReceived(String s) {
        String cmd = s.substring(0, 4);
        log.info("Received: " + s);
        if (cmd.equals("stat")) {
            int hour = Integer.parseInt(s.substring(4, 6), 10);
            int minute = Integer.parseInt(s.substring(6, 8), 10);
            int onoff = Integer.parseInt(s.substring(8, 9), 10);

            log.info("Setting time to " + hour + ":" + minute + ", enabled to " + onoff);
            runOnUiThread(() -> setTime(hour, minute));
            runOnUiThread(() -> setSwitchedOn(onoff == 1));
            log.info("Done setting");
        } else {
            log.info("Unknown command " + cmd);
        }
    }

    private void setSwitchedOn(boolean on) {
        log.info("Set switched_on to " + on);
        onSwitch.setChecked(on);
    }

    private void setTime(int h, int m) {
        log.info("Set time to " + h + ":" + m);
        String time = String.format("%02d:%02d", h, m);
        hours = h;
        minutes = m;
        timeDisplay.setText(time);
    }

    @Override
    protected void onStop() {
        tryingToConnect = false;
        if (bluetoothDevice != null) {
            bluetoothManager.closeDevice(bluetoothDevice);
        }
        super.onStop();
    }
}
