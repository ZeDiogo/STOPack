package pt.ulisboa.tecnico.diic.stopack;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.Set;

public class SearchBluetooth extends AppCompatActivity {
    private static final String TAG = "SearchBluetooth";
    private static final int REQUEST_BLUETOOTH = 1;
    private ListView devicesView;
    private BluetoothAdapter bluetoothAdapter;
    private ArrayList discoveredDevices = new ArrayList<String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "OnCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_search_bluetooth);
        devicesView = (ListView) findViewById(R.id.devices);

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        enableBluetooth();

        //showPairedDevices();
        showAvailableDevices();
    }

    private final BroadcastReceiver bReciever = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // Create a new device item
                discoveredDevices.add(device.getName() + " : " + device.getAddress());
                Log.d("Discovered Devices: ", discoveredDevices.toString());
                //DeviceItem newDevice = new DeviceItem(device.getName(), device.getAddress(), "false");
                // Add it to our adapter
                //bluetoothAdapter.add(newDevice);
            }
        }
    };

    private void showAvailableDevices() {
        start();
        devicesView.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, discoveredDevices));
        Log.d("SLEEP", "0000000000000000000000000000000000000000");
        try{Thread.sleep(5000);} catch (InterruptedException ie) {ie.printStackTrace();}
        Log.d("SLEEP", discoveredDevices.toString());
        cancel();
    }

    private void cancel() {
        this.unregisterReceiver(bReciever);
        bluetoothAdapter.cancelDiscovery();
    }

    private void start() {
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        //bluetoothAdapter.clear();
        this.registerReceiver(bReciever, filter);
        bluetoothAdapter.startDiscovery();
    }

    private void showPairedDevices() {
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        ArrayList pairedDevicesList = new ArrayList<String>();
        for (BluetoothDevice device : pairedDevices) {
            pairedDevicesList.add(device.getName() + " : " + device.getAddress());
            Log.d(TAG, "Device: " + device.getName());
        }
        devicesView.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, pairedDevicesList));
    }

    private void enableBluetooth() {
        if (bluetoothAdapter == null) {
            new AlertDialog.Builder(this)
                    .setTitle("Not compatible")
                    .setMessage("Your phone does not support Bluetooth")
                    .setPositiveButton("Exit", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            System.exit(0);
                        }
                    })
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .show();
        } else if (!bluetoothAdapter.isEnabled()) {
            Intent enableBT = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBT, REQUEST_BLUETOOTH);
        }
    }

    /**
     * This method is required for all devices running API23+
     * Android must programmatically check the permissions for bluetooth. Putting the proper permissions
     * in the manifest is not enough.
     *
     * NOTE: This will only execute on versions > LOLLIPOP because it is not needed otherwise.
     */
    private void checkBTPermissions() {
        if(Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP){
            int permissionCheck = this.checkSelfPermission("Manifest.permission.ACCESS_FINE_LOCATION");
            permissionCheck += this.checkSelfPermission("Manifest.permission.ACCESS_COARSE_LOCATION");
            if (permissionCheck != 0) {

                this.requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, 1001); //Any number
            }
        }else{
            Log.d(TAG, "checkBTPermissions: No need to check permissions. SDK version < LOLLIPOP.");
        }
    }
}
