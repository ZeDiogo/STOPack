package pt.ulisboa.tecnico.diic.stopack;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

public class Display extends AppCompatActivity {
    //public static final String EXTRA_MESSAGE = "com.example.myfirstapp.MESSAGE";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_configure_pack);
        Intent intent = new Intent(this, ConfigurePack.class);
        startActivity(intent);
    }

    /** Called when the user taps the Send button */
    /*public void sendMessage(View view) {
        Intent intent = new Intent(this, ConfigurePack.class);
        //EditText editText = (EditText) findViewById(R.id.editText);
        //String message = editText.getText().toString();
        //intent.putExtra(EXTRA_MESSAGE, message);
        startActivity(intent);
    }

    public void goToLimitScreen(View view) {
        Intent intent = new Intent(this, ConfigurePack.class);
        startActivity(intent);
    }*/

}
