<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
// Caminho do arquivo para salvar os dados
$arquivo = __DIR__ . "/dados.json";

// Se recebeu POST com JSON do ESP32
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = file_get_contents("php://input");
    // O ESP32 envia UM objeto JSON por POST, mas você precisa salvar um ARRAY
    // de todos os sensores. No seu código atual do ESP32, ele só envia o último
    // sensor que ele processou, sobrescrevendo o arquivo.
    // Vamos manter o seu código PHP como está para a solução mais rápida,
    // que é salvar o objeto JSON único.

    if ($input) {
        // Salva os dados brutos JSON recebidos
        if (file_put_contents($arquivo, $input)) {
            echo "OK"; // Resposta que o ESP32 espera (código 200 + corpo "OK")
        } else {
            // Em caso de erro de permissão ou escrita no arquivo
            http_response_code(500); // Código de erro interno do servidor
            echo "Erro ao salvar os dados.";
        }
    } else {
        http_response_code(400); // Bad Request
        echo "Nenhum dado recebido";
    }
    exit;
} else {
    // Se tentarem acessar este arquivo diretamente pelo navegador (GET)
    http_response_code(405); // Método não permitido
    echo "Método de acesso não permitido.";
}
// OBS: Você precisa trocar o serverUrl no ESP32 para "http://192.168.0.21/sensor/receber.php"
?>