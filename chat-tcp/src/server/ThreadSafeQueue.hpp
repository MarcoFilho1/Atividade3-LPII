#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <semaphore>
#include <atomic>
#include <optional>
#include <string>

class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t capacity)
      : capacity_(capacity), slots_(capacity), items_(0) {}

    // Produção bloqueante (respeita bounded queue)
    bool push(std::string msg, std::atomic_bool& running) {
        // se estiver finalizando, não bloqueia eternamente
        while (running.load()) {
            if (slots_.try_acquire()) {
                {
                    std::lock_guard<std::mutex> lk(m_);
                    q_.push(std::move(msg));
                }
                items_.release();
                cv_.notify_one();
                return true;
            }
            // espera curta para permitir shutdown
            std::unique_lock<std::mutex> lk(cv_m_);
            cv_.wait_for(lk, std::chrono::milliseconds(50));
        }
        return false;
    }

    // Consumo bloqueante (retorna std::nullopt no shutdown)
    std::optional<std::string> pop(std::atomic_bool& running) {
        for (;;) {
            if (!running.load()) {
                // drena se ainda há itens
                if (items_.try_acquire()) {
                    std::lock_guard<std::mutex> lk(m_);
                    auto s = std::move(q_.front());
                    q_.pop();
                    slots_.release();
                    return s;
                }
                return std::nullopt;
            }
            if (items_.try_acquire()) {
                std::lock_guard<std::mutex> lk(m_);
                auto s = std::move(q_.front());
                q_.pop();
                slots_.release();
                return s;
            }
            std::unique_lock<std::mutex> lk(cv_m_);
            cv_.wait_for(lk, std::chrono::milliseconds(50));
        }
    }

    // acorda consumidores/produtores
    void notify_all() { cv_.notify_all(); }

private:
    size_t capacity_;
    std::queue<std::string> q_;
    std::mutex m_;

    std::counting_semaphore<> slots_; // espaços livres
    std::counting_semaphore<> items_; // itens disponíveis

    std::mutex cv_m_;
    std::condition_variable cv_;
};
