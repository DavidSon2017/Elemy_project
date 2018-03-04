package com.example.youkyung.myapplication;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.Typeface;
import android.media.Image;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.FragmentActivity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Timer;
import java.util.TimerTask;


public class MainActivity extends FragmentActivity {
    public static final int startfloor = 1;
    public static final int finalfloor = 40; //1층~40층 빌딩

    int flag = 0;
    Socket socket;
    BufferedReader sock_in;
    PrintWriter sock_out;

    final String serverIP = "192.168.43.126";
    final int serverPort = 8888;

    ImageButton upbut;
    ImageButton downbut;
    ImageButton power;
    TextView status;
    EditText floor;
    TextView clock;
    String ctos = ""; //클라이언트에서 서버로 전송할 메시지(client to server)
    String stoc = ""; //서버에서 전송된 메시지를 저장할 변수

    //시간 핸들러. 1초마다 호출됨
    public static final int UI_UPDATE = 100;
    public Handler clohand = new Handler(){
       public void handleMessage(Message msg) {
           switch (msg.what) {
               case UI_UPDATE: {
                   GregorianCalendar mCalendar = new GregorianCalendar();
                   Calendar now = Calendar.getInstance();

                   SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
                   String str = dateFormat.format(mCalendar.getTime());

                   clock.setText(str);
               }
               break;
               default:
                   break;
           }
       }
    };

    //실제 서버에서 접속해서 데이터를 전송하고 전송받을 스레드
    class ConnectThread extends Thread{
        public void run(){
            try{
                socket = new Socket(serverIP,serverPort);//데이터 전송을 위한 스트림 생성
                sock_out = new PrintWriter(socket.getOutputStream(),true);
                sock_in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                while(true){
                    stoc = sock_in.readLine(); //서버로부터 값 읽어옴 (층수)
                    status.post(new Runnable() {
                        @Override
                        public void run() {
                            floor.setText("00");
                            if(stoc.equals("up_move")) {
                                status.setTextSize(TypedValue.COMPLEX_UNIT_SP, 80);
                                status.setText("▲ MOVING..");
                                upbut.setEnabled(false);
                                downbut.setEnabled(false);
                                String str = "엘리베이터가 움직입니다.";
                                Toast.makeText(MainActivity.this,str, Toast.LENGTH_SHORT).show();

                                upbut.setBackgroundResource(R.drawable.up_button_on);
                                downbut.setBackgroundResource(R.drawable.down_button_on);
                            }
                            else if(stoc.equals("down_move")){
                                status.setTextSize(TypedValue.COMPLEX_UNIT_SP, 80);
                                status.setText("▼ MOVING..");
                                upbut.setEnabled(false);
                                downbut.setEnabled(false);
                                String str = "엘리베이터가 움직입니다.";
                                Toast.makeText(MainActivity.this,str, Toast.LENGTH_SHORT).show();

                                upbut.setBackgroundResource(R.drawable.up_button_on);
                                downbut.setBackgroundResource(R.drawable.down_button_on);
                            }
                            else {
                                status.setTextSize(TypedValue.COMPLEX_UNIT_SP, 150);
                                status.setText(stoc); //엘리베이터 층수 표시
                                upbut.setEnabled(true);
                                downbut.setEnabled(true);

                                upbut.setBackgroundResource(R.drawable.up_button);
                                downbut.setBackgroundResource(R.drawable.down_button);
                            }
                        }
                    });
                }
            }catch(Exception e){
                e.printStackTrace();
            }
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        upbut = (ImageButton)findViewById(R.id.upbut);
        downbut = (ImageButton)findViewById(R.id.downbut);
        power = (ImageButton)findViewById(R.id.power);
        status = (TextView)findViewById(R.id.status);
        floor = (EditText) findViewById(R.id.call);

        clock = (TextView)findViewById(R.id.clock);
        final Timer timer;
        TimerTask timerTask;
        timer = new Timer(true);
        timerTask = new TimerTask(){
            public void run(){
                clohand.sendEmptyMessage(UI_UPDATE);
            }

            public boolean cancel(){
                return super.cancel();
            }
        };
        timer.schedule(timerTask,0,1000);

        Typeface digit_font = Typeface.createFromAsset(getAssets(),"DS-DIGI.TTF");
        status.setTypeface(digit_font);
        floor.setTypeface(digit_font);
        clock.setTypeface(digit_font);

        int color = Color.parseColor("#007644");
        floor.getBackground().setColorFilter(color, PorterDuff.Mode.SRC_IN);


        ConnectThread th = new ConnectThread();
        th.start();

        floor.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                floor.setText(""); //공백으로 바뀌어 숫자의 입력을 수월하게 함
            }
        });

        power.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                moveTaskToBack(true);
                finish();
                android.os.Process.killProcess(android.os.Process.myPid());
            }
        });

        upbut.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                InputMethodManager imm =  (InputMethodManager)getSystemService(INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(floor.getWindowToken(), 0);

                int num = Integer.parseInt(floor.getText().toString().trim());
                if(num<startfloor || num>finalfloor){
                    String str = startfloor +" ~ "+finalfloor+" 의 숫자를 입력하세요.";
                    Toast.makeText(MainActivity.this,str, Toast.LENGTH_SHORT).show();
                }
                else {
                    sock_out.println("0");
                    ctos = "" + num;
                    String str = ctos + "층으로 호출합니다.";
                    Toast.makeText(MainActivity.this, str, Toast.LENGTH_SHORT).show();
                    sock_out.println(ctos);
                }
            }
        });


        downbut.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                InputMethodManager imm =  (InputMethodManager)getSystemService(INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(floor.getWindowToken(), 0);

                int num = Integer.parseInt(floor.getText().toString().trim());
                if(num<startfloor || num>finalfloor){
                    String str = startfloor +" ~ "+finalfloor+" 의 숫자를 입력하세요.";
                    Toast.makeText(MainActivity.this,str, Toast.LENGTH_SHORT).show();
                }
                else {
                    sock_out.println("0");
                    ctos = "" + num;
                    String str = ctos + "층으로 호출합니다.";
                    Toast.makeText(MainActivity.this, str, Toast.LENGTH_SHORT).show();
                    sock_out.println(ctos);
                }
            }
        });
    }

    @Override
    protected void onStop() {
        super.onStop();
        try {
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
