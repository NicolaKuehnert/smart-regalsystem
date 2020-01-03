package com.example.reader;

import androidx.appcompat.app.AppCompatActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class bookshelf extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bookshelf);


    }

    public void backClick(View view){
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
        finish();
    }

    /*
     * Sends the text in TextView searchText_bookshelf to the m5-stack if connection is active
     */
    public void searchClick(View v){
        TextView tv = findViewById(R.id.searchText_bookshelf);

        if(BLEConnection.pubBLE == null){
            Toast.makeText(this,"Please connect to m5-stack first", Toast.LENGTH_SHORT).show();
            return;
        }

        BLEConnection.pubBLE.sendUpdate(tv.getText().toString());
    }
}
