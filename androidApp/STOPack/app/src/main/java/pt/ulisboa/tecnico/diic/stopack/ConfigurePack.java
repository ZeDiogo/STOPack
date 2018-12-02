package pt.ulisboa.tecnico.diic.stopack;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class ConfigurePack extends AppCompatActivity {

    private int _limit = 10;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_configure_pack);
        this.updateLimit();
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
        //TODO
    }

    private void updateLimit() {
        TextView view_limit = (TextView)findViewById(R.id.int_limit);
        view_limit.setText(String.valueOf(_limit));
    }
}
