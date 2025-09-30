<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <title>Status da Vaga Inteligente</title>
</head>
<body>

<h1>Monitoramento da Vaga</h1>
<div id="status-container">
    Carregando status...
</div>

<?php
// PHP para buscar os dados brutos (opcional, mas bom para o primeiro carregamento)
$arquivo = __DIR__ . "/dados.json";
$dados_json = "";

if (file_exists($arquivo)) {
    $dados_json = file_get_contents($arquivo);
}
?>

<script>
// Função para carregar e exibir os dados
function atualizarStatus() {
    const container = document.getElementById('status-container');
    
    // Faz uma requisição GET para o próprio index.php ou para um arquivo JSON
    // A maneira mais simples é carregar o arquivo dados.json diretamente
    fetch('dados.json')
        .then(response => {
            // Verifica se a resposta foi bem-sucedida
            if (!response.ok) {
                throw new Error('Erro ao carregar dados.json: ' + response.statusText);
            }
            return response.json();
        })
        .then(data => {
            // Se o JSON foi carregado com sucesso, atualiza o HTML
            container.innerHTML = `
                <p><strong>Vaga:</strong> ${data.vaga}</p>
                <p><strong>Status:</strong> <span style="font-weight: bold; color: ${data.status === 'LIVRE' ? 'green' : 'red'};">${data.status}</span></p>
                <p><small>Última atualização: ${new Date().toLocaleTimeString()}</small></p>
            `;
        })
        .catch(error => {
            // Exibe um erro se algo der errado (ex: arquivo não existe ou JSON inválido)
            console.error('Erro de requisição:', error);
            container.innerHTML = `<p style="color: red;">Erro ao carregar dados: ${error.message}</p>`;
        });
}

// 1. Chama a função imediatamente para carregar o status inicial
atualizarStatus();

// 2. Chama a função a cada 5000 milissegundos (5 segundos)
// Isso sincroniza com o delay(5000) do seu ESP32
setInterval(atualizarStatus, 5000); 

</script>

</body>
</html>