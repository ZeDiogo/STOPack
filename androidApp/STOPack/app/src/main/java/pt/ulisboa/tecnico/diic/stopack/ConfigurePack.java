package pt.ulisboa.tecnico.diic.stopack;

import android.bluetooth.BluetoothAdapter;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class ConfigurePack extends AppCompatActivity {

    private int _limit = 10;
    private static final String TAG = "SearchBluetooth";
    private Bluetooth bt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_configure_pack);
        this.updateLimit();
        bt = new Bluetooth(this, mHandler);
        connectService();

    }

    public void increment(View view) {
        _limit += 1;
        this.updateLimit();
    }

    public void decrement(View view) {
        _limit -=1;
        if (_limit < 0) _limit = 0;
        this.updateLimit();
    }

    public void sendLimitToPack(View view) {
        bt.sendMessage(String.valueOf(_limit));
    }

    private void updateLimit() {
        TextView view_limit = (TextView)findViewById(R.id.int_limit);
        view_limit.setText(String.valueOf(_limit));
    }

    public void connectService(){
        try {
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            if (bluetoothAdapter.isEnabled()) {
                bt.start();
                bt.connectDevice("RNBT-89CC");
                Log.d(TAG, "Btservice started - listening");

            } else {
                Log.w(TAG, "Btservice started - bluetooth is not enabled");

            }
        } catch(Exception e){
            Log.e(TAG, "Unable to start bt ",e);

        }
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Bluetooth.MESSAGE_STATE_CHANGE:
                    Log.d(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                    break;
                case Bluetooth.MESSAGE_WRITE:
                    Log.d(TAG, "MESSAGE_WRITE ");
                    break;
                case Bluetooth.MESSAGE_READ:
                    Log.d(TAG, "MESSAGE_READ ");
                    break;
                case Bluetooth.MESSAGE_DEVICE_NAME:
                    Log.d(TAG, "MESSAGE_DEVICE_NAME "+msg);
                    break;
                case Bluetooth.MESSAGE_TOAST:
                    Log.d(TAG, "MESSAGE_TOAST "+msg);
                    break;
            }
        }
    };

}
