package com.example.reader;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void bookshelfClick(View view) {
        Intent intent = new Intent(this, bookshelf.class);
        startActivity(intent);
    }

    public void exitClick(View view) {
        System.exit(0);
    }

    public void settingsClick(View view){
        Intent intent = new Intent(this, settings.class);
        startActivity(intent);
    }
}
