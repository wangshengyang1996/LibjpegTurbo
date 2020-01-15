package com.wsy.libjpegturbo;

import androidx.appcompat.app.AppCompatActivity;

import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;

import com.arcsoft.imageutil.ArcSoftImageFormat;
import com.arcsoft.imageutil.ArcSoftImageUtil;
import com.arcsoft.imageutil.ArcSoftImageUtilError;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

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
        try {
            Log.i(TAG, "onCreate: " + getResources().getString(getPackageManager().getPackageInfo(getPackageName(),0).applicationInfo.labelRes));
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        imageViewForLibjpegTurbo = findViewById(R.id.iv_show_image_libjpeg_turbo);
        imageViewForSystem = findViewById(R.id.iv_show_image_system);
        testBitmapJpegTransform();
        testNv21JpegTransform();
        try {
            testNv21JpegTransformSrcJni();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


    private void testBitmapJpegTransform() {
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

    private void testNv21JpegTransformSrcJni() throws IOException {

    }

    private void testNv21JpegTransform() {
        try {
            InputStream stream = getAssets().open("flash_500x312.NV21");
            byte[] nv21 = new byte[stream.available()];
            stream.read(nv21);
            stream.close();
            JpegCompressor jpegCompressor = new JpegCompressor();
            JpegDecompressor jpegDecompressor = new JpegDecompressor();
            if (jpegCompressor.createCompressor() && jpegDecompressor.createDecompressor()) {
                byte[] i420 = new byte[nv21.length];
                ArcSoftImageUtil.transformImage(nv21, i420, 500, 312, ArcSoftImageFormat.NV21, ArcSoftImageFormat.I420);
                long start = System.currentTimeMillis();
                byte[] jpeg = jpegCompressor.i420ToJpeg(i420, 500, 312, 100);
                long cost = System.currentTimeMillis() - start;
                Log.i(TAG, "jpegCompressor.i420ToJpeg cost is : " + cost);
                start = System.currentTimeMillis();
                i420 = jpegDecompressor.jpegToI420(jpeg);
                cost = System.currentTimeMillis() - start;
                Log.i(TAG, "jpegCompressor.jpegToI420 cost is : " + cost);
                Bitmap bitmap = Bitmap.createBitmap(500, 312, Bitmap.Config.ARGB_8888);
                int code = ArcSoftImageUtil.imageDataToBitmap(i420, bitmap, ArcSoftImageFormat.I420);
                if (code != ArcSoftImageUtilError.CODE_SUCCESS) {
                    Log.e(TAG, "testNv21JpegTransform: transform failed");
                    return;
                }

                Log.i(TAG, "testNv21JpegTransform: " + bitmap.getWidth() + "x" + bitmap.getHeight());
                imageViewForLibjpegTurbo.setImageBitmap(bitmap);

            }
            jpegDecompressor.destroyDecompressor();
            jpegCompressor.destroyCompressor();


        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
