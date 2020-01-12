package com.wsy.libjpegturbo;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.ByteArrayOutputStream;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    // 显示libjpeg处理过的图像
    ImageView imageViewForLibjpegTurbo;
    // 显示使用系统API处理过的图像
    ImageView imageViewForSystem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        imageViewForLibjpegTurbo = findViewById(R.id.iv_show_image_libjpeg_turbo);
        imageViewForSystem = findViewById(R.id.iv_show_image_system);
        test();

    }

    private void test() {
        Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.ic_the_flash);
        JpegCompressor jpegCompressor = new JpegCompressor();
        JpegDecompressor jpegDecompressor = new JpegDecompressor();
        if (jpegCompressor.createCompressor() && jpegDecompressor.createDecompressor()) {
            // use libjpeg-turbo
            long start = System.currentTimeMillis();
            byte[] bytes = jpegCompressor.bitmapToJpeg(bitmap, JpegCompressor.TJSAMP_420, 50);
            long cost = System.currentTimeMillis() - start;
            if (bytes != null) {
                start = System.currentTimeMillis();
                Bitmap newBmp = jpegDecompressor.jpegToBitmap(bytes);
                long decompressCost = System.currentTimeMillis() - start;
                Log.i(TAG, " libjpeg-turbo, jpegLength = " + bytes.length + ", width = " + newBmp.getWidth() + ", height = " + newBmp.getHeight() + ", compressCost = " + cost);
                Log.i(TAG, " libjpeg-turbo, decompressCost = " + decompressCost);
                imageViewForLibjpegTurbo.setImageBitmap(newBmp);
            }

            // use system

            start = System.currentTimeMillis();
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            boolean res = bitmap.compress(Bitmap.CompressFormat.JPEG, 50, baos);
            bytes = baos.toByteArray();
            if (!res) {
                Log.e(TAG, "onCreate: compress failed");
            }
            cost = System.currentTimeMillis() - start;
            start = System.currentTimeMillis();
            Bitmap newBmp = BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
            long decompressCost = System.currentTimeMillis() - start;

            Log.i(TAG, " system, jpegLength = " + bytes.length + ", width = " + newBmp.getWidth() + ", height = " + newBmp.getHeight() + ", compressCost = " + cost);
            Log.i(TAG, " system, decompressCost = " + decompressCost);
            imageViewForSystem.setImageBitmap(newBmp);


        } else {
            Log.e(TAG, "failed to create compressor and decompressor");
        }

        jpegDecompressor.destroyDecompressor();
        jpegCompressor.destroyCompressor();
    }
}
