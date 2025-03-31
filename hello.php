<?php

require_once 'VietQR.php'; 

$qr = new VietQR();

$stk = "huykhong";
$ctk = "TRINH GIA HUY";
$nganhang_bin = "970423";
$so_tien = "50000";
$noi_dung = "Thanh toan hoa don xyz";

// Thiết lập dữ liệu
$qr->setBeneficiaryOrganization($nganhang_bin, $stk) // Mã ngân hàng & số tài khoản
   ->setTransactionAmount($so_tien)                        // Số tiền chuyển
   ->setAdditionalDataFieldTemplate($noi_dung);      // Nội dung chuyển khoản

// Tạo mã QR string
$qrString = $qr->build();

// In ra kết quả
//echo "QR String: " . $qrString;

echo "[H1]CTK:".$ctk."\r\n";
echo "\r\n";
echo "Ngan hang:\r\n";
echo "[H2]".ngan_hang($nganhang_bin)."\r\n";
echo "\r\n";
echo "STK:\r\n";
echo "[H2]".$stk."\r\n";
echo "\r\n";
echo "So tien:\r\n";
echo "[H2]".formatVND($so_tien)."\r\n";
echo "[QR]".$qrString;

function ngan_hang($bin_code){
    //https://developers.zalo.me/docs/zalo-notification-service/phu-luc/danh-sach-bin-code
    $ngan_hang = [
            "970416" => "ACB",
            "970415" => "Vietin",
            "970423" => "TPBank",
            "970422" => "MBBank"
        ];
    return $ngan_hang[$bin_code] ?? '';
}

function formatVND($amount) {
    $formatted = number_format($amount, 0, ',', '.');
    return $formatted . 'vnd';
}

?>