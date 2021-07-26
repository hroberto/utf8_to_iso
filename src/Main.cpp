#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/locale.hpp>
// #include <locale>


struct PARAMETERS {
    std::string fileName;
}  g_param;



// #############  PROTOTIPOS DE FUNCOES  #############

bool  valid_parameters(int argc, char** argv);
bool is_utf8(const std::string & mesg_ );
std::string transforme_utf_to_iso( const std::string & textIn_ );
std::string transforme_char_special( const std::string & textIn_, const std::string toChar_, const bool extendedChar_ = false );

// #############



int main(int argc, char** argv)
{

    if( ! valid_parameters( argc, argv ) ) {
        return ( EXIT_FAILURE );
    }

    std::string buffer;
    std::ifstream fin( g_param.fileName.c_str() );

	if ( fin.is_open()) {
        while ( std::getline(fin, buffer) ) {

            buffer = transforme_utf_to_iso( buffer );
            buffer = transforme_char_special( buffer, "" , true);

            std::cout << buffer << std::endl;

        }
        fin.close();
	}


    // return ( EXIT_FAILURE );

    return ( EXIT_SUCCESS );
}


/**
 * \brief  Valida os parametros informados via linha de comando.
 * \param  argc Quantidade de parametros informados na chamada do sistema
 * \param  argv Lista com os valores dos parametros informados
 * \return Retorno TRUE/FALSE da validação dos parametros.
 **/
bool  valid_parameters(int argc, char** argv)
{
    if ( argc != 2) {
        std::cerr << "Parametros invalidos."  << std::endl;
        std::cerr << " Entre: " << argv[ 0 ] << " < FileName >"  << std::endl;
        std::cerr << std::endl;
        return ( false );
    }

    g_param.fileName =  argv[ 1 ];

    return ( true );
}


/**
 * \brief  Realiza a conversao do texto para ISO 8583, caso o texto seja em UTF8.
 * \param  textIn_  Texto de origem informado para conversao.
 * \return Texto em ISO-8583 
 * \info   Caso o texto nao necessite de conversao, a mensagem de retorno sera identica a original.
 **/
std::string transforme_utf_to_iso( const std::string & textIn_ )
{
    // std::locale loc = boost::locale::generator().generate("");
    // std::locale::global(loc);

    // Use latin1 to convert string to UTF8
    // std::string utf8 = boost::locale::conv::to_utf<char>( textIn_.c_str(), "Latin1");
    // return ( utf8 );
    if ( is_utf8( textIn_ )) {
        // std::string iso = boost::locale::conv::from_utf<char>( textIn_.c_str(), "Latin1");
        std::string iso = boost::locale::conv::from_utf<char>( textIn_.c_str(), "ISO8859-1" );
        return ( iso );
    }

    return ( textIn_ );
}



/**
 * \brief  Realiza o tratamento de alguns caracteres especiais que não são exibiveis no sistema.
 * \param  textIn_  Texto de entrada para tratamento.
 * \param  toChar_  Caracter para a substituicao dos caracteres da mensagem original.
 * \param  extendedChar_ Identifica se a conversao ira tratar os codigos de caracteres entre 128 e 255 da tabela ASCII.
 * \return Texto convertido pela funcao.
 **/
std::string transforme_char_special( const std::string & textIn_, const std::string toChar_, const bool extendedChar_ )
{
    std::stringstream ss;

	for (size_t i = 0; i < textIn_.size(); i++)
	{
		unsigned char c = textIn_.at(i);

        switch (c)
		{
		case '\t':
		case '\r':
		case '\n':
            ss << c;
            break;

		default:

            if ( c<32 ) {
                ss << toChar_;
            }
            else if( extendedChar_ && c>127 && c<161 ) {
                ss << toChar_;
            }
            else if( ! extendedChar_ && c>127 ) {
                ss << toChar_;
            }
            else {
                ss << c;
            }
        }
    }


    return ( ss.str() );
}


/**
 * \brief  Verifica se a mensagem possui algum caracter em UTF-8.
 * \param  mesg_ Texto de entrada para verificacao.
 * \return Retorno booleano ( true/false ) identificando a presenca de caracteres em UTF-8
 **/
bool is_utf8(const std::string & mesg_ )
{
    if( mesg_.empty() )
        return ( false );

    const unsigned char * bytes = (const unsigned char *)mesg_.c_str();

    while(*bytes)
    {
        if( (// ASCII
             // use bytes[0] <= 0x7F to allow ASCII control characters
                bytes[0] == 0x09 ||
                bytes[0] == 0x0A ||
                bytes[0] == 0x0D ||
                (0x20 <= bytes[0] && bytes[0] <= 0x7E)
            )
        ) {
            bytes += 1;
            continue;
        }

        if( (// non-overlong 2-byte
                (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF)
            )
        ) {
            bytes += 2;
            continue;
        }

        if( (// excluding overlongs
                bytes[0] == 0xE0 &&
                (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// straight 3-byte
                ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                    bytes[0] == 0xEE ||
                    bytes[0] == 0xEF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// excluding surrogates
                bytes[0] == 0xED &&
                (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            )
        ) {
            bytes += 3;
            continue;
        }

        if( (// planes 1-3
                bytes[0] == 0xF0 &&
                (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// planes 4-15
                (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// plane 16
                bytes[0] == 0xF4 &&
                (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            )
        ) {
            bytes += 4;
            continue;
        }

        return ( false );
    }

    return ( true );
}
