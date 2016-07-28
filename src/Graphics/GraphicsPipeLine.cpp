#include <./Graphics/GraphicsPipeLine.h>

GraphicsPipeLine::GraphicsPipeLine(){
	this->status.reserve(8);  // 動的に増減させる限界容量
	this->status.resize(8);   // 実際の配列の指定
}
GraphicsPipeLine::GraphicsPipeLine(unsigned long size){
	this->status.reserve(size); // vectorの最大のサイズを変える
	this->status.resize(size);
}
GraphicsPipeLine::~GraphicsPipeLine(){}

void GraphicsPipeLine::setStatus(){
	// パイプラインにステータスの設定
	for (auto v : this->status){
		if (v)v->SetStatus();
	}
}

bool GraphicsPipeLine::Draw(){
	// パイプラインにステータスの設定
	this->setStatus();
	// 描画呼び出し
	return false;
}
bool GraphicsPipeLine::Attach(GraphicStatus* ptr){
	for (auto &v : this->status){
		if (v == nullptr){
			ptr->AddRef(); // 参照カウンタを1ずつ増やす
			v = ptr;
			return true;
		}
		else if (v == ptr){
			return false;
		}
	}
	return false;
}
bool GraphicsPipeLine::Remove(GraphicStatus* ptr){
	for (auto &v : this->status){
		if (v == ptr){
			v->Release();
			v = nullptr;
			return true;
		}
	}
	return false;
}

void GraphicsPipeLine::Release(){
	for (auto &v : this->status){
		if (v){
			v->Release();
			v = nullptr;
		}
	}
}
