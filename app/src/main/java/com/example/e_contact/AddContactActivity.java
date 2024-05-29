package com.example.e_contact;



import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class AddContactActivity extends AppCompatActivity {

    private EditText contactNameEditText;
    private EditText contactNumberEditText;
    private Button addButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.add_item);

        contactNameEditText = findViewById(R.id.contact_name);
        contactNumberEditText = findViewById(R.id.contact_number);
        addButton = findViewById(R.id.add_button);

        addButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String contactName = contactNameEditText.getText().toString();
                String contactNumber = contactNumberEditText.getText().toString();

                if (contactName.isEmpty() || contactNumber.isEmpty()) {
                    Toast.makeText(AddContactActivity.this, "Please fill in all fields", Toast.LENGTH_SHORT).show();
                } else {
                    int result = DbHelper.insertContact(contactName, contactNumber);

                    if (result == 0) {
                        Toast.makeText(AddContactActivity.this, "Contact added successfully", Toast.LENGTH_SHORT).show();
                        finish(); // Close the activity
                    } else if (result == -1) {
                        Toast.makeText(AddContactActivity.this, "Contact number already exists", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(AddContactActivity.this, "Failed to add contact", Toast.LENGTH_SHORT).show();
                    }
                }
            }
        });
    }
}
