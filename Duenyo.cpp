/**
 * Project Untitled
 */


#include "Duenyo.h"


/**
 * Duenyo implementation
 */


Duenyo::Duenyo(string nombre, string dni, string art, bool cambioArt, float difArt, float TotCobrar, list <Mercaderia*> MisProductos, list <Herramientas_Alquiler*> MisHerras) :Persona(nombre,dni) {
    this->QueArticulo = art;
    this->CambioArticulo = cambioArt;
    this->DiferenciaArt = difArt;
    this->Dinero = TotCobrar;
    this->ListaProductos = MisProductos;
    this->ListaHerramientas = MisHerras;


}

void Duenyo::AtenderCliente (Cliente Client, Cerrajero Cerra, Plomero plomo, Despachante despache){

    bool estadoFerreteria = Ferreteria::CartelAbiertoCerrado();
    //estadoFerreteria = true; //PARA PROBARLO SI ES HORARIO EN EL QUE ESTA CERRADO!!
    string mensaje = (estadoFerreteria == true) ? "Estamos Abiertos" : "Estamos Cerrados";
    cout << mensaje << endl;


    bool estadoPago;

    if(estadoFerreteria == true){ //si estamos abiertos
        cout<<"Atendiendo a: " << Client.get_nombre()<<endl;

        bool copiaLlave = false;
        if(Client.get_cerrajero()){
            copiaLlave = Cerra.HacerCopiaLlave(Client);
            string mensaje2 = (copiaLlave == true) ? "Se hizo la copia de la llave" : "No se hizo la copia de la llave";
            cout << mensaje2 << endl;
        }

        if(Client.get_despachante()){
            despache.EntregarPedido(Client);
        }

        if(Client.get_plomero()){
            plomo.resolverProblema();
        }


        try {
                estadoPago = CobrarYDarVuelto(Client);
        }
        catch (NoFondos  &e) {
            cout<< e.what()<< endl;
            return;
        }

        string mensaje2 = (estadoPago == true) ? "Se cobro con exito" : "No se pudo cobrar";
        cout << mensaje2 << endl;
    }

    return;
}


const bool Duenyo:: IdentificarArticuloDeFoto(Cliente Cli) {
    bool tengo = false;
    if(Cli.get_fotoArt() == true){
        tengo = rand()% (0-1)+1;
    }
    return tengo;

    // si el cliente trajo foto: cliente le muestra la foto al duenyo y el mismo dice si tiene el acticulo o no
}

bool Duenyo::CobrarYDarVuelto(Cliente Cli) {

    bool estadoPago = false;
    float dineroCaja;
    float dineroCliente;
    dineroCaja = get_dinero();
    dineroCliente = Cli.get_dinero();

    float total = 0;

    total = total + generarPresupuesto(Cli);
    total = total + AlquilerHerramienta(Cli);
    total = total + DiferenciaArticulo(Cli); //calculo el total de todo

    cout<<"El total es = " << total << endl;

    //chequeo que el cliente tenga fondos:
    if(dineroCliente >= total){
        dineroCliente = dineroCliente - total;
        Cli.set_dinero(dineroCliente); //resto el pago al cliente

        dineroCaja = dineroCaja + total;
        set_dinero(dineroCaja); //sumo el pago a la caja

        estadoPago = true;
    }

    if(estadoPago == false){
        throw NoFondos();
    }

    //si el cliente no tiene suficiente dinero el pago no se puede efectuar

    return estadoPago;
}

float Duenyo::AlquilerHerramienta(Cliente Cli) {

    float total = 0;
    list<Herramientas_Alquiler*>::iterator it;
    string HerrCli = Cli.get_HerrAlq();
    //cuanto tiempo lo uso y por ende cuanto es el alquiler   

    if(Cli.get_Alquilo()){ //si alquilo una herramienta
        for (it = ListaHerramientas.begin(); it != ListaHerramientas.end(); it++){
            if((*it)->get_tipoHerrAlq() == HerrCli){

                (*it)->set_devSeg(true);
                total = (*it)->get_tiempoUso() * (*it)->get_precio() - (*it)->get_PrecioSeg(); //si esta en bueas condiciones se devuelve el seguro
            }

            else{
                (*it)->set_devSeg(false);
                total = (*it)->get_tiempoUso() * (*it)->get_precio(); //si esta en malas condiciones no se devuelve el seguro (no se lo cobra aca porque ya se lo cobran antes)
            }

        }

    }

    return total;
}


float Duenyo::DiferenciaArticulo(Cliente Cli) {

    float diferencia = 0;
    float precioViejoArticulo = Cli.get_PrecioArtViejo();
    float precioActualArticulo = 0;

    list<Mercaderia*>::iterator it;
    list<Mercaderia*> productos = get_ListaProducts();
    int stockActual;

    if (Cli.get_Cambio() == true) {
        for (it = productos.begin(); it != productos.end(); ++it) {
            if(((*it)->get_nombreMerc() == Cli.get_ArtViejo())){
                if((*it)->get_Stock() > 0){ // Chequeo si hay stock
                    precioActualArticulo = (*it)->get_Precio();
                    diferencia = precioActualArticulo- precioViejoArticulo;
                    cout<<"Diferencia de precio = "<< diferencia <<endl;
                    stockActual = (*it)->get_Stock() -1;
                    (*it)->set_Stock(stockActual); //resto el articulo del stock
                }
            }
        }
    }
    return diferencia;
}


float Duenyo::generarPresupuesto(Cliente Cli) {
    float presup = 0.0; // Creo contador del presupuesto y lo inicializo

    list<Mercaderia*>::iterator it;
    const list<string>& listaQuiero = Cli.get_ListaQueQuiero();
    int stockActual;

    for (it = ListaProductos.begin(); it != ListaProductos.end(); it++) { // Recorro la lista de productos de la ferreteria
        for (const string& producto : listaQuiero) { // Recorro la lista de productos que quiere el cliente
            if ((*it)->get_nombreMerc() == producto) { // Si son el mismo producto, sumo el precio al total
               if((*it)->get_Stock() > 0){ // Chequeo si hay stock

                   presup += (*it)->get_Precio();
                   stockActual = (*it)->get_Stock() -1;
                   (*it)->set_Stock(stockActual); //resto el articulo del stock
                   cout<<"Hay stock de "<< producto <<endl;
                   cout << **it << endl; // Imprimir el objeto Mercaderia mediante sobrecarga
                }

            else
                   cout<<"No hay stock de "<< producto <<endl;
            }
        }
    }

    return presup; // Devuelvo presup
}


void Duenyo::set_queArt(string art) {
    this->QueArticulo = art;
    return;

}
const string Duenyo::get_queArt() {
    return this->QueArticulo;
}


void Duenyo::set_cambArt(bool cambioArt) {
    this->CambioArticulo = cambioArt;
    return;
}

const bool Duenyo::get_cambArt() {
    return this->CambioArticulo;
}


void Duenyo::set_difArt(float difArt) {
    this->DiferenciaArt = difArt;
    return;

 }

const float Duenyo::get_difArt() {
    return this->DiferenciaArt;
}


void Duenyo::set_dinero(float TotCobrar) {
    this->Dinero = TotCobrar;
    return;
}

const float Duenyo::get_dinero() {
    return this->Dinero;
}


void Duenyo:: set_ListaProducts(list <Mercaderia*> MisProductos){
    this->ListaProductos = MisProductos;

    return;
}

const list <Mercaderia*> Duenyo:: get_ListaProducts(){
    return this->ListaProductos;
}

void Duenyo::set_ListaHerramientas(list <Herramientas_Alquiler*> MisHerras){
    this->ListaHerramientas = MisHerras;
    return;
}

const list <Herramientas_Alquiler*> Duenyo:: get_ListaHerramientas(){
     return this->ListaHerramientas;
}


Duenyo::~Duenyo() {

}

void Duenyo::imprimirStock(list <Mercaderia*> MisProductos){
    list<Mercaderia*>::iterator it;
    for (it = MisProductos.begin(); it != MisProductos.end(); it++) {
        cout << *(*it) << endl;
    }
}
